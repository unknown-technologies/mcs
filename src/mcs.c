#include <mcs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

static unsigned long get_time(void)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

void MCSInit(MCS* mcs, int width, int height)
{
	memset(mcs, 0, sizeof(MCS));

	mcs->width = width;
	mcs->height = height;

	GXInit();
	GXSetView(0, 0, width, height);

	MTInit(&mcs->mt);

	if(!SNDInit()) {
		printf("Failed to initialize sound system\n");
		exit(1);
	}

	CFGRead(&mcs->config);

	UIInit(&mcs->ui, mcs);
	UICreateClock(&mcs->ui);

	/* initialize next alarm such that it's in the future */
	time(&mcs->clock.next_alarm);
	mcs->clock.next_alarm += 10;
}

void MCSFree(MCS* mcs)
{
	AIDestroy();
	UIDestroy(&mcs->ui);
	MTClose(&mcs->mt);
	CFGDestroy(&mcs->config);
}

static void ALRMProcess(MCSClock* clk)
{
	time_t t;
	time(&t);

	struct tm alarm_tm = *localtime(&t);
	alarm_tm.tm_hour = clk->alarm.hour;
	alarm_tm.tm_min = clk->alarm.minute;
	alarm_tm.tm_sec = 0;
	time_t alarm_t = mktime(&alarm_tm);

	if(alarm_t < t) {
		alarm_tm.tm_mday++;
		alarm_t = mktime(&alarm_tm);
	}

	if(clk->enabled && clk->next_alarm <= t) {
		/* trigger the alarm */
		if(!clk->triggered) {
			/* only trigger the alarm sound ONCE instead of
			 * every frame within the second when the alarm is
			 * supposed to be triggered ...
			 */
			SNDPlayAlarm();
		}
		clk->triggered = TRUE;
	}

	clk->next_alarm = alarm_t;
}

void MCSProcess(MCS* mcs)
{
	ALRMProcess(&mcs->clock);

	MTPoll(&mcs->mt);
	UIProcess(&mcs->ui);
}

static unsigned long last_time = 0;
static unsigned long frames = 0;
static unsigned long fps = 0;

void MCSRender(MCS* mcs)
{
	unsigned long now = get_time();
	unsigned long dt = now - last_time;
	if(dt >= 1000) {
		last_time = now;
		fps = frames;
		frames = 0;

		if(fps > 0 && fps < 60) {
			printf("FPS drop: %lu\n", fps);
		}
	} else {
		frames++;
	}

#ifdef SHOW_FPS
	float color[4] = { 1, 1, 1, 1 };
	char buf[32];

	sprintf(buf, "FPS: %lu", fps);
	GXDrawMicroText(mcs->width - strlen(buf) * 6, mcs->height - 9,
			color, buf);
#endif

	UIDraw(&mcs->ui);
}
