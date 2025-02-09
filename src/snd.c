#include <string.h>
#include <mcs.h>

extern const DSPHeader alarmL[];
extern const DSPHeader alarmR[];

static ADPCM dsp_alarm_l;
static ADPCM dsp_alarm_r;

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
	if(play_alarm && !last_play_alarm) {
		DSPInit(&dsp_alarm_l, alarmL);
		DSPInit(&dsp_alarm_r, alarmR);
	}

	last_play_alarm = play_alarm;

	if(play_alarm) {
		DSPDecode(&dsp_alarm_l, buffer, samples, 2);
		DSPDecode(&dsp_alarm_r, buffer + 1, samples, 2);
	} else {
		memset(buffer, 0, samples * 2 * sizeof(s16));
	}
}
