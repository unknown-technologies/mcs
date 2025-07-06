#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <mcs/types.h>

#include <pthread.h>

#define	OpDMX		0x5000

#define	DMX_MAXCH	32
#define	DMX_MAXFIX	32

typedef struct {
	char	header[8];
	u16	opcode;
	u16	protover;
	u8	sequence;
	u8	physical;
	u16	universe;
	u16	paramcnt;
	u8	parameters[512];
} ArtDMX;

typedef struct {
	unsigned int	channels;
	unsigned int	address;
	u8		faders[DMX_MAXCH];
	u8		params[DMX_MAXCH];
} DMXFixture;

typedef struct {
	DMXFixture	fixtures[DMX_MAXFIX];

	volatile int	fd;
	unsigned int	universe;

	pthread_t	thread;
	volatile BOOL	quit_thread;
	volatile BOOL	quit_ack;

	volatile int	master;
} DMX;

void	ARTDMXInit(ArtDMX* dmx, u8 sequence, u8 physical, u16 universe,
		u16 paramcnt, u8* parameters);
BOOL	ARTDMXSend(ArtDMX* dmx, int fd);

BOOL	DMXInit(DMX* dmx);
void	DMXDestroy(DMX* dmx);
BOOL	DMXSetTarget(DMX* dmx, const char* hostname);

#endif
