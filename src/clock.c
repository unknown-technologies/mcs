#include <mcs.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

static const float color_white[4] = { 1.0, 1.0, 1.0, 1.0 };
static const float color_gray[4]  = { 0.5, 0.5, 0.5, 1.0 };
static const float color_red[4]   = { 1.0, 0.0, 0.0, 1.0 };
static const float color_green[4] = { 0.0, 1.0, 0.0, 1.0 };

static const char* weekdays[] = { "Sunday", "Monday", "Tuesday",
	"Wednesday", "Thursday", "Friday", "Saturday" };

void UIInitClock(UIPanel* self)
{
	MCSClock* clk = &UIGetMCS(self)->clock;
	clk->editing = FALSE;

	setenv("TZ", "Europe/Vienna", 1);
	tzset();
}

static void UIProcessClockAlarm(UIPanel* self, float ignore_y)
{
	MCS* mcs = UIGetMCS(self);
	MCSClock* clk = &mcs->clock;

	float width = GXGetTextWidth(&mcs->deface, 192.0, "DIS  ENA") +
		GXGetTextWidthAlt(&mcs->deface, 192.0, "00:00");
	float size = (float) mcs->width / width * 192.0;
	float height = GXGetFontHeight(&mcs->deface, size);
	float pos_y = (mcs->height - height) / 2.0;

	float pos_x = GXGetTextWidth(&mcs->deface, size, "DIS ");
	float pos_dis = 0;
	float width_dis = pos_x;

	float alarm_x_h = pos_x;
	float alarm_x_m = alarm_x_h +
		GXGetTextWidthAlt(&mcs->deface, size, "00:");
	float alarm_width = GXGetTextWidthAlt(&mcs->deface, size, "00");

	pos_x += GXGetTextWidthAlt(&mcs->deface, size, "00:00");
	float pos_ena = pos_x;
	float width_ena = GXGetTextWidth(&mcs->deface, size, " ENA");

	float offset = GXGetFontHeight(&mcs->deface, size) * 0.75;
	float button_inc = pos_y - offset;
	float button_dec = pos_y + offset +
		GXGetFontHeight(&mcs->deface, size);

	float button_end = mcs->height / 2.0;

	for(unsigned int i = 0; i < MTGetSlotCount(&mcs->mt); i++) {
		MT_SLOT* slot = MTGetSlot(&mcs->mt, i);
		if(!slot || !slot->active) {
			continue;
		}

		if(slot->y <= ignore_y) {
			continue;
		}

		/* everything below is about single touch buttons */
		if(!slot->fresh) {
			continue;
		}

		/* check if touch point is on "DIS" */
		if(slot->x >= pos_dis &&
				slot->x <= (pos_dis + width_dis) &&
				slot->y >= pos_y &&
				slot->y <= pos_y + GXGetFontHeight(&mcs->deface, size)) {
			clk->enabled = FALSE;
			clk->triggered = FALSE;
			SNDStopAlarm();
		}

		/* check if touch point is on "ENA" */
		if(slot->x >= pos_ena &&
				slot->x <= (pos_ena + width_ena) &&
				slot->y >= pos_y &&
				slot->y <= pos_y + GXGetFontHeight(&mcs->deface, size)) {
			clk->enabled = TRUE;
		}

		/* check if touch point is on "++" (hour) */
		if(slot->x >= alarm_x_h &&
				slot->x <= (alarm_x_h + alarm_width) &&
				slot->y >= button_inc &&
				slot->y <= button_end) {
			clk->alarm.hour = (clk->alarm.hour + 1) % 24;
		}

		/* check if touch point is on "++" (minute) */
		if(slot->x >= alarm_x_m &&
				slot->x <= (alarm_x_m + alarm_width) &&
				slot->y >= button_inc &&
				slot->y <= button_end) {
			clk->alarm.minute = (clk->alarm.minute + 1) % 60;
		}

		/* check if touch point is on "--" (hour) */
		if(slot->x >= alarm_x_h &&
				slot->x <= (alarm_x_h + alarm_width) &&
				slot->y >= button_end &&
				slot->y <= button_dec) {
			clk->alarm.hour--;
			if(clk->alarm.hour < 0) {
				clk->alarm.hour = 23;
			}
		}

		/* check if touch point is on "--" (hour) */
		if(slot->x >= alarm_x_m &&
				slot->x <= (alarm_x_m + alarm_width) &&
				slot->y >= button_end &&
				slot->y <= button_dec) {
			clk->alarm.minute--;
			if(clk->alarm.minute < 0) {
				clk->alarm.minute = 59;
			}
		}
	}
}

void UIProcessClock(UIPanel* self)
{
	MCS* mcs = UIGetMCS(self);
	MCSClock* clk = &mcs->clock;

	float width = GXGetTextWidth(&mcs->deface, 192.0,
			"ALRM  ===> EDIT ")
		+ GXGetTextWidthAlt(&mcs->deface, 192.0, "00:0000.00.0000");
	float size = (float) mcs->width / width * 192.0;

	float pos_x = GXGetTextWidth(&mcs->deface, size, "ALRM ");
	pos_x += GXGetTextWidthAlt(&mcs->deface, size, "00:00");

	width = GXGetTextWidth(&mcs->deface, size, " ===> EDIT ");

	for(unsigned int i = 0; i < MTGetSlotCount(&mcs->mt); i++) {
		MT_SLOT* slot = MTGetSlot(&mcs->mt, i);
		if(!slot || !slot->active || !slot->fresh) {
			continue;
		}

		/* check if touch point is on "===> EDIT" */
		if(slot->x >= pos_x && slot->x <= (pos_x + width) &&
				slot->y <= GXGetFontHeight(&mcs->deface, size)) {
			clk->editing = !clk->editing;
		}
	}

	if(clk->editing) {
		UIProcessClockAlarm(self,
				GXGetFontHeight(&mcs->deface, size));
	}
}

static void UIDrawClockTime(UIPanel* self, struct tm* tm)
{
	char buf[32];
	MCS* mcs = UIGetMCS(self);
	MCSClock* clk = &mcs->clock;

	BOOL is_red = FALSE;
	if(clk->triggered) {
		struct timespec t;
		clock_gettime(CLOCK_REALTIME, &t);
		is_red = t.tv_nsec / 1000000 < 500;
	}

	const float* color = is_red ? color_red : color_white;

	sprintf(buf, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
	float width = GXGetTextWidthAlt(&mcs->deface,
			mcs->deface.font->height, buf);
	float size = mcs->width / width * mcs->deface.font->height;
	float height = GXGetFontHeight(&mcs->deface, size);
	float time_y = (mcs->height - height) / 2.0;
	GXDrawTextAlt(&mcs->deface, 0, time_y, size, color, buf);
}

static void UIDrawClockAlarm(UIPanel* self)
{
	char buf[32];
	MCS* mcs = UIGetMCS(self);
	MCSClock* clk = &mcs->clock;

	float width = GXGetTextWidth(&mcs->deface, 192.0, "DIS  ENA") +
		GXGetTextWidthAlt(&mcs->deface, 192.0, "00:00");
	float size = (float) mcs->width / width * 192.0;
	float height = GXGetFontHeight(&mcs->deface, size);
	float pos_y = (mcs->height - height) / 2.0;

	float pos_x = 0;
	GXDrawText(&mcs->deface, pos_x, pos_y, size, color_red, "DIS");
	pos_x += GXGetTextWidth(&mcs->deface, size, "DIS ");

	float alarm_x_h = pos_x;
	float alarm_x_m = alarm_x_h +
		GXGetTextWidthAlt(&mcs->deface, size, "00:");

	sprintf(buf, "%02d:%02d", clk->alarm.hour, clk->alarm.minute);
	GXDrawTextAlt(&mcs->deface, pos_x, pos_y, size, color_white, buf);
	pos_x += GXGetTextWidthAlt(&mcs->deface, size, buf);
	pos_x += GXGetTextWidth(&mcs->deface, size, " ");

	GXDrawText(&mcs->deface, pos_x, pos_y, size, color_green, "ENA");

	float offset = GXGetFontHeight(&mcs->deface, size) * 0.75;
	float button_inc = pos_y - offset;
	float button_dec = pos_y + offset;

	GXDrawTextAlt(&mcs->deface, alarm_x_h, button_inc, size, color_red,
			"++");
	GXDrawTextAlt(&mcs->deface, alarm_x_m, button_inc, size, color_red,
			"++");

	GXDrawTextAlt(&mcs->deface, alarm_x_h, button_dec, size, color_red,
			"--");
	GXDrawTextAlt(&mcs->deface, alarm_x_m, button_dec, size, color_red,
			"--");
}

void UIDrawClock(UIPanel* self)
{
	char buf[32];

	MCS* mcs = UIGetMCS(self);
	MCSClock* clk = &mcs->clock;

	time_t t;
	time(&t);
	struct tm tm = *localtime(&t);

	/* center */
	if(clk->editing) {
		UIDrawClockAlarm(self);
	} else {
		UIDrawClockTime(self, &tm);
	}

	/* date */
	sprintf(buf, "%02d.%02d.%04d", tm.tm_mday, tm.tm_mon + 1,
			tm.tm_year + 1900);

	float width = GXGetTextWidth(&mcs->deface, 192.0,
			"ALRM  ===> EDIT ")
		+ GXGetTextWidthAlt(&mcs->deface, 192.0, "00:0000.00.0000");
	float size = (float) mcs->width / width * 192.0;
	width = GXGetTextWidthAlt(&mcs->deface, size, buf);
	float date_x = mcs->width - width;
	GXDrawTextAlt(&mcs->deface, date_x, 0, size, color_white, buf);

	/* alarm */
	const float* alarm_color = clk->enabled ? color_green : color_gray;
	sprintf(buf, "%02d:%02d", clk->alarm.hour, clk->alarm.minute);
	float pos_x = GXGetTextWidth(&mcs->deface, size, "ALRM ");
	GXDrawText(&mcs->deface, 0, 0, size, alarm_color, "ALRM");
	GXDrawTextAlt(&mcs->deface, pos_x, 0, size, alarm_color, buf);
	pos_x += GXGetTextWidthAlt(&mcs->deface, size, buf);
	pos_x += GXGetTextWidth(&mcs->deface, size, " ");

	const float* edit_color = clk->editing ? color_red : color_green;
	GXDrawText(&mcs->deface, pos_x, 0, size, edit_color, "===> EDIT");

	/* weekday */
	float weekday_x = 10;
	float weekday_y = mcs->height - GXGetFontHeight(&mcs->deface, size);
	GXDrawText(&mcs->deface, weekday_x, weekday_y, size, color_white,
			weekdays[tm.tm_wday]);

	/* remaining time */
	time_t dt = clk->next_alarm - t;
	int seconds = dt % 60;
	dt /= 60;
	int minutes = dt % 60;
	int hours = dt / 60;

	sprintf(buf, "%02d:%02d:%02d", hours, minutes, seconds);
	width = GXGetTextWidthAlt(&mcs->deface, size, buf);
	float center_x = (mcs->width - width / 2.0) / 2.0;
	GXDrawTextAlt(&mcs->deface, center_x, weekday_y, size, color_gray,
			buf);
}

static UIPanelDefinition clock_dlg = {
	.init = UIInitClock,
	.destroy = NULL,
	.process = UIProcessClock,
	.draw = UIDrawClock
};

void UICreateClock(UI* ui)
{
	UICreatePanel(ui, &clock_dlg);
}
