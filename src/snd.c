#include <string.h>
#include <mcs.h>

extern const DSPHeader DSP_alarmL[];
extern const DSPHeader DSP_alarmR[];

static ADPCM alarm_l;
static ADPCM alarm_r;

static volatile BOOL play_alarm;
static volatile BOOL last_play_alarm;

static void SNDiFillBuffer(s16* buffer, unsigned int frames);

BOOL SNDInit(void)
{
	play_alarm = FALSE;
	last_play_alarm = FALSE;

	return AIInit(SNDiFillBuffer);
}

void SNDPlayAlarm(void)
{
	play_alarm = TRUE;
}

void SNDStopAlarm(void)
{
	play_alarm = FALSE;
}

static void SNDiFillBuffer(s16* buffer, unsigned int samples)
{
	/* alarm handling */
	if(play_alarm && !last_play_alarm) {
		DSPInit(&alarm_l, DSP_alarmL);
		DSPInit(&alarm_r, DSP_alarmR);
	}

	last_play_alarm = play_alarm;

	if(play_alarm) {
		/* NOTE: alarm MUST NOT be a one-shot, otherwise there might
		 * be stale samples in the buffer when the sound ends. */
		DSPDecode(&alarm_l, buffer, samples, 2);
		DSPDecode(&alarm_r, buffer + 1, samples, 2);
	} else {
		memset(buffer, 0, samples * 2 * sizeof(s16));
	}
}
