#include <mcs.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <alsa/asoundlib.h>

#define	FRAMES		1024
#define	CHANNELS	2

static snd_pcm_t* handle = NULL;

static const char* device = "default";
static volatile BOOL quit_thread = FALSE;
static volatile BOOL quit_ack = FALSE;
static pthread_t thread_id;

static s16 buffer[FRAMES * CHANNELS];

static void	AIiFillBuffer(void);
static void*	AIiThread(void* arg);

BOOL AIInit(void)
{
	int err;

	if((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK,
					0)) < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		handle = NULL;
		return FALSE;
	}

	if((err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE,
					SND_PCM_ACCESS_RW_INTERLEAVED,
					CHANNELS, 48000, 0, 100000)) < 0) {
		printf("Failed to select sample format: %s\n",
				snd_strerror(err));
		AIDestroy();
		return FALSE;
	}

	/* create audio thread */
	err = pthread_create(&thread_id, NULL, &AIiThread, NULL);
	if(err) {
		printf("Failed to create audio thread: %s\n",
				strerror(err));
		AIDestroy();
		return FALSE;
	}

	return TRUE;
}

void AIDestroy(void)
{
	if(handle) {
		quit_thread = TRUE;
		while(!quit_ack);

		snd_pcm_close(handle);
		handle = NULL;
	}
}

/******************************************************************************/
/* internal functions                                                         */
/******************************************************************************/
extern const DSPHeader alarmL[];
extern const DSPHeader alarmR[];

static ADPCM dsp_alarm_l;
static ADPCM dsp_alarm_r;

static volatile BOOL play_alarm = FALSE;
static volatile BOOL last_play_alarm = FALSE;

void AIPlayAlarm(void)
{
	play_alarm = TRUE;
}

void AIStopAlarm(void)
{
	play_alarm = FALSE;
}

static void AIiFillBuffer(void)
{
	if(play_alarm && !last_play_alarm) {
		DSPInit(&dsp_alarm_l, alarmL);
		DSPInit(&dsp_alarm_r, alarmR);
	}

	last_play_alarm = play_alarm;

	if(play_alarm) {
		DSPDecode(&dsp_alarm_l, buffer, FRAMES, 2);
		DSPDecode(&dsp_alarm_r, buffer + 1, FRAMES, 2);
	} else {
		memset(buffer, 0, sizeof(buffer));
	}
}

static void* AIiThread(void* arg)
{
	int err;

	(void) arg;

	while(!quit_thread) {
		AIiFillBuffer();

		snd_pcm_sframes_t frames = snd_pcm_writei(handle, buffer,
				FRAMES);

		if(frames < 0) {
			frames = snd_pcm_recover(handle, frames, 0);
		}

		if(frames < 0) {
			printf("snd_pcm_writei failed: %s\n",
					snd_strerror(frames));
			break;
		}

		if(frames > 0 && frames < FRAMES) {
			printf("Short write (expected %i, wrote %li)\n",
					FRAMES, frames);
		}
	}

	if((err = snd_pcm_drain(handle)) < 0) {
		printf("snd_pcm_drain failed: %s\n", snd_strerror(err));
	}

	quit_ack = TRUE;

	return NULL;
}

