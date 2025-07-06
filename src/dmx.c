#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#include <mcs.h>

static void*	DMXiThread(void* arg);

BOOL DMXInit(DMX* dmx)
{
	memset(dmx, 0, sizeof(DMX));
	dmx->fd = -1;

	dmx->quit_thread = FALSE;
	dmx->quit_ack = FALSE;

	int err = pthread_create(&dmx->thread, NULL, &DMXiThread, dmx);
	if(err) {
		printf("Failed to create DMX thread: %s\n", strerror(err));
		dmx->quit_ack = TRUE;
		DMXDestroy(dmx);
		return FALSE;
	}

	/* TODO: make this configurable */
	dmx->universe = 1;
	for(unsigned int i = 0; i < 3; i++) {
		DMXFixture* fix = &dmx->fixtures[i];
		fix->channels = 3;
		fix->address = i * 3;
	}
	return DMXSetTarget(dmx, "172.16.10.42");
}

void DMXDestroy(DMX* dmx)
{
	int fd = dmx->fd;
	dmx->fd = -1;

	dmx->quit_thread = TRUE;
	while(!dmx->quit_ack);

	if(fd != -1) {
		close(fd);
	}
}

/* TODO: implement UI notifications for errors */
BOOL DMXSetTarget(DMX* dmx, const char* hostname)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;	/* allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_NUMERICSERV;
	hints.ai_protocol = 0; /* any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	struct addrinfo* result;

	int s = getaddrinfo(hostname, "6454", &hints, &result);
	if(s) {
		printf("getaddrinfo: %s\n", gai_strerror(s));
		return FALSE;
	}

	int sock = -1;
	struct addrinfo* rp;
	for(rp = result; rp; rp = rp->ai_next) {
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(sock == -1) {
			continue;
		}

		if(connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
		}

		close(sock);
	}

	freeaddrinfo(result);

	if(!rp) {
		printf("Could not connect\n");
		return FALSE;
	}

	int oldsock = dmx->fd;
	dmx->fd = sock;
	if(oldsock != -1) {
		close(oldsock);
	}

	return TRUE;
}

/******************************************************************************/
/* internal functions                                                         */
/******************************************************************************/
static void* DMXiThread(void* arg)
{
	DMX* dmx = (DMX*) arg;
	u8 params[512];

	while(!dmx->quit_thread) {
		/* refresh rate: 40Hz */
		usleep(1000000 / 40);

		/* no open ArtNet connection: do nothing */
		if(dmx->fd == -1) {
			continue;
		}

		float scale = dmx->master / 255.0f;
		for(unsigned int i = 0; i < DMX_MAXFIX; i++) {
			DMXFixture* fix = &dmx->fixtures[i];

			/* no channels: this fixture is inactive */
			if(fix->channels == 0) {
				continue;
			}

			/* assumption: fix->address + fix->params <= 512 */
			for(unsigned int n = 0; n < fix->channels; n++) {
				int value = fix->params[n];
				int param = (int) roundf(value * scale);
				params[fix->address + n] = param;
			}
		}

		ArtDMX artdmx;
		ARTDMXInit(&artdmx, 0, 0, dmx->universe, 512, params);
		ARTDMXSend(&artdmx, dmx->fd);
	}

	dmx->quit_ack = TRUE;

	return NULL;
}
