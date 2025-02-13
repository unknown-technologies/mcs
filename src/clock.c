#include <mcs.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

static const char* weekdays[] = { "Sunday", "Monday", "Tuesday",
	"Wednesday", "Thursday", "Friday", "Saturday" };

extern const char TEX_gear[];
static GXTexture gear_texture;

void UIInitClock(UIPanel* self)
{
	MCSClock* clk = &UIGetMCS(self)->clock;
	clk->editing = FALSE;

	GXCreateTexture(&gear_texture, (const TXTR*) TEX_gear);

	setenv("TZ", "Europe/Vienna", 1);
	tzset();

	UISetFullscreen(self, TRUE);
}

void UIDestroyClock(UIPanel* self)
{
	(void) self;

	GXDestroyTexture(&gear_texture);
}

static void UIProcessClockAlarm(UIPanel* self, float ignore_y)
{
	MCS* mcs = UIGetMCS(self);
	MCSClock* clk = &mcs->clock;
	GXFont* deface = &UIGet(self)->deface;

	float width = GXGetTextWidth(deface, 192.0, "DIS  ENA") +
		GXGetTextWidthAlt(deface, 192.0, "00:00");
	float size = (float) mcs->width / width * 192.0;
	float height = GXGetFontHeight(deface, size);
	float pos_y = (mcs->height - height) / 2.0;

	float pos_x = GXGetTextWidth(deface, size, "DIS ");
	float pos_dis = 0;
	float width_dis = pos_x;

	float alarm_x_h = pos_x;
	float alarm_x_m = alarm_x_h +
		GXGetTextWidthAlt(deface, size, "00:");
	float alarm_width = GXGetTextWidthAlt(deface, size, "00");

	pos_x += GXGetTextWidthAlt(deface, size, "00:00");
	float pos_ena = pos_x;
	float width_ena = GXGetTextWidth(deface, size, " ENA");

	float offset = GXGetFontHeight(deface, size) * 0.75;
	float button_inc = pos_y - offset;
	float button_dec = pos_y + offset +
		GXGetFontHeight(deface, size);

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
				slot->y <= pos_y + GXGetFontHeight(deface, size)) {
			clk->enabled = FALSE;
			clk->triggered = FALSE;
			SNDStopAlarm();
		}

		/* check if touch point is on "ENA" */
		if(slot->x >= pos_ena &&
				slot->x <= (pos_ena + width_ena) &&
				slot->y >= pos_y &&
				slot->y <= pos_y + GXGetFontHeight(deface, size)) {
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
	GXFont* deface = &UIGet(self)->deface;

	float width = GXGetTextWidth(deface, 192.0,
			"ALRM  ===> EDIT ")
		+ GXGetTextWidthAlt(deface, 192.0, "00:0000.00.0000");
	float size = (float) mcs->width / width * 192.0;

	float pos_x = GXGetTextWidth(deface, size, "ALRM ");
	pos_x += GXGetTextWidthAlt(deface, size, "00:00");

	width = GXGetTextWidth(deface, size, " ===> EDIT ");

	float gear_x = mcs->width - size;
	float gear_y = mcs->height - size;

	for(unsigned int i = 0; i < MTGetSlotCount(&mcs->mt); i++) {
		MT_SLOT* slot = MTGetSlot(&mcs->mt, i);
		if(!slot || !slot->active || !slot->fresh) {
			continue;
		}

		/* check if touch point is on "===> EDIT" */
		if(slot->x >= pos_x && slot->x <= (pos_x + width) &&
				slot->y <= GXGetFontHeight(deface, size)) {
			clk->editing = !clk->editing;
		}

		if(slot->x >= gear_x && slot->x <= (gear_x + size) &&
				slot->y >= gear_y && slot->y <= (gear_y + size)) {
			UICreateDesktop(UIGet(self));
		}
	}

	if(clk->editing) {
		UIProcessClockAlarm(self, GXGetFontHeight(deface, size));
	}
}

static void UIDrawClockTime(UIPanel* self, struct tm* tm)
{
	char buf[32];
	MCS* mcs = UIGetMCS(self);
	MCSClock* clk = &mcs->clock;
	GXFont* deface = &UIGet(self)->deface;

	BOOL is_red = FALSE;
	if(clk->triggered) {
		struct timespec t;
		clock_gettime(CLOCK_REALTIME, &t);
		is_red = t.tv_nsec / 1000000 < 500;
	}

	const float* color = is_red ? color_red : color_white;

	sprintf(buf, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
	float width = GXGetTextWidthAlt(deface, deface->font->height, buf);
	float size = mcs->width / width * deface->font->height;
	float height = GXGetFontHeight(deface, size);
	float time_y = (mcs->height - height) / 2.0;
	GXDrawTextAlt(deface, 0, time_y, size, color, buf);
}

static void UIDrawClockAlarm(UIPanel* self)
{
	char buf[32];
	MCS* mcs = UIGetMCS(self);
	MCSClock* clk = &mcs->clock;
	GXFont* deface = &UIGet(self)->deface;

	float width = GXGetTextWidth(deface, 192.0, "DIS  ENA") +
		GXGetTextWidthAlt(deface, 192.0, "00:00");
	float size = (float) mcs->width / width * 192.0;
	float height = GXGetFontHeight(deface, size);
	float pos_y = (mcs->height - height) / 2.0;

	float pos_x = 0;
	GXDrawText(deface, pos_x, pos_y, size, color_red, "DIS");
	pos_x += GXGetTextWidth(deface, size, "DIS ");

	float alarm_x_h = pos_x;
	float alarm_x_m = alarm_x_h +
		GXGetTextWidthAlt(deface, size, "00:");

	sprintf(buf, "%02d:%02d", clk->alarm.hour, clk->alarm.minute);
	GXDrawTextAlt(deface, pos_x, pos_y, size, color_white, buf);
	pos_x += GXGetTextWidthAlt(deface, size, buf);
	pos_x += GXGetTextWidth(deface, size, " ");

	GXDrawText(deface, pos_x, pos_y, size, color_green, "ENA");

	float offset = GXGetFontHeight(deface, size) * 0.75;
	float button_inc = pos_y - offset;
	float button_dec = pos_y + offset;

	GXDrawTextAlt(deface, alarm_x_h, button_inc, size, color_red, "++");
	GXDrawTextAlt(deface, alarm_x_m, button_inc, size, color_red, "++");

	GXDrawTextAlt(deface, alarm_x_h, button_dec, size, color_red, "--");
	GXDrawTextAlt(deface, alarm_x_m, button_dec, size, color_red, "--");
}

void UIDrawClock(UIPanel* self)
{
	char buf[32];

	MCS* mcs = UIGetMCS(self);
	MCSClock* clk = &mcs->clock;
	GXFont* deface = &UIGet(self)->deface;

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

	float width = GXGetTextWidth(deface, 192.0, "ALRM  ===> EDIT ")
		+ GXGetTextWidthAlt(deface, 192.0, "00:0000.00.0000");
	float size = (float) mcs->width / width * 192.0;
	width = GXGetTextWidthAlt(deface, size, buf);
	float date_x = mcs->width - width;
	GXDrawTextAlt(deface, date_x, 0, size, color_white, buf);

	/* alarm */
	const float* alarm_color = clk->enabled ? color_green : color_gray;
	sprintf(buf, "%02d:%02d", clk->alarm.hour, clk->alarm.minute);
	float pos_x = GXGetTextWidth(deface, size, "ALRM ");
	GXDrawText(deface, 0, 0, size, alarm_color, "ALRM");
	GXDrawTextAlt(deface, pos_x, 0, size, alarm_color, buf);
	pos_x += GXGetTextWidthAlt(deface, size, buf);
	pos_x += GXGetTextWidth(deface, size, " ");

	const float* edit_color = clk->editing ? color_red : color_green;
	GXDrawText(deface, pos_x, 0, size, edit_color, "===> EDIT");

	/* weekday */
	float weekday_x = 10;
	float weekday_y = mcs->height - GXGetFontHeight(deface, size);
	GXDrawText(deface, weekday_x, weekday_y, size, color_white,
			weekdays[tm.tm_wday]);

	/* remaining time */
	time_t dt = clk->next_alarm - t;
	int seconds = dt % 60;
	dt /= 60;
	int minutes = dt % 60;
	int hours = dt / 60;

	sprintf(buf, "%02d:%02d:%02d", hours, minutes, seconds);
	width = GXGetTextWidthAlt(deface, size, buf);
	float center_x = (mcs->width - width / 2.0) / 2.0;
	GXDrawTextAlt(deface, center_x, weekday_y, size, color_gray, buf);

	/* render gear icon */
	const float pos_size[4] = {
		mcs->width - size, mcs->height - size,
		size, size
	};
	glActiveTexture(GL_TEXTURE0);
	GXUseTexture(&gear_texture);
	GXRenderTexQuad(pos_size, color_gray, TRUE);

}

static UIPanelDefinition clock_dlg = {
	.init = UIInitClock,
	.destroy = UIDestroyClock,
	.process = UIProcessClock,
	.draw = UIDrawClock
};

void UICreateClock(UI* ui)
{
	UICreatePanel(ui, &clock_dlg);
}
