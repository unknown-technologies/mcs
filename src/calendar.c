#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <mcs.h>

static const char* month_names[12] = {
	"January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December"
};

static const char* weekday_names[7] = {
	"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"
};

typedef struct {
	float	size;
	float	cellwidth;
	float	space;
	float	pos_x;
} Calendar;

void UIInitCalendar(UIPanel* self)
{
	UISetTitle(self, "Calendar");

	Calendar* cal = (Calendar*) malloc(sizeof(Calendar));
	UISetUserData(self, cal);

	GXFont* deface = &UIGet(self)->deface;

	float maxwidth = 0;
	for(unsigned int i = 0; i < 7; i++) {
		float width = GXGetTextWidth(deface, 192.0,
				weekday_names[i]);
		if(width > maxwidth) {
			maxwidth = width;
		}
	}
	float date_width = GXGetTextWidthAlt(deface, 192.0, "00");
	if(date_width > maxwidth) {
		maxwidth = date_width;
	}

	float space = GXGetTextWidth(deface, 192.0, " ");

	float height = GXGetFontLineHeight(deface, 192.0) * 8;
	float scale = UIGetHeight(self) / height;
	float width = (maxwidth * 7 + space * 6) * scale;
	if(width > UIGetWidth(self)) {
		scale = UIGetWidth(self) / (maxwidth * 7 + space * 6);
	}

	cal->size = 192.0 * scale;
	cal->cellwidth = maxwidth * scale;
	cal->space = space * scale;
	cal->pos_x = (UIGetWidth(self) - (7 * maxwidth + 6 * space) * scale)
		/ 2.0f;
}

void UIDestroyCalendar(UIPanel* self)
{
	Calendar* cal = (Calendar*) UIGetUserData(self);
	free(cal);
}

void UIDrawCalendar(UIPanel* self)
{
	Calendar* cal = (Calendar*) UIGetUserData(self);
	GXFont* deface = &UIGet(self)->deface;

	time_t t;
	time(&t);
	struct tm tm = *localtime(&t);

	/* current month/year */
	char buf[32];
	sprintf(buf, "%s %04d", month_names[tm.tm_mon],
			tm.tm_year + 1900);

	float pos_x = (UIGetWidth(self) -
			GXGetTextWidth(deface, cal->size, buf)) / 2.0f;
	float height = GXGetFontLineHeight(deface, cal->size);

	GXDrawText(deface, pos_x, 0, cal->size, color_orange, buf);

	/* weekday header */
	pos_x = cal->pos_x;
	for(unsigned int i = 0; i < 7; i++) {
		const char* name = weekday_names[i];
		float width = GXGetTextWidth(deface, cal->size, name);
		float cx = (cal->cellwidth - width) / 2.0f;

		GXDrawText(deface, pos_x + cx, height, cal->size,
				color_white, name);
		pos_x += cal->cellwidth + cal->space;
	}

	/* table */
	int wday = CALGetWeekday(tm.tm_year + 1900, tm.tm_mon + 1, 1);
	unsigned int days = CALGetDaysOfMonth(tm.tm_year + 1900,
			tm.tm_mon + 1);
	pos_x = cal->pos_x + (cal->cellwidth + cal->space) * wday;
	float pos_y = 2 * height;
	unsigned int day = wday;
	for(unsigned int i = 0; i < days; i++) {
		sprintf(buf, "%02d", i + 1);

		float width = GXGetTextWidthAlt(deface, cal->size, buf);
		float cx = (cal->cellwidth - width) / 2.0f;

		const float* color = color_gray;
		if(i == tm.tm_mday - 1) {
			color = color_orange;
		}

		GXDrawTextAlt(deface, pos_x + cx, pos_y, cal->size, color,
				buf);

		if(day < 6) {
			day++;
			pos_x += cal->cellwidth + cal->space;
		} else {
			pos_x = cal->pos_x;
			pos_y += height;
			day = 0;
		}
	}
}

static UIPanelDefinition calendar_dlg = {
	.init = UIInitCalendar,
	.destroy = UIDestroyCalendar,
	.process = NULL,
	.draw = UIDrawCalendar
};

void UICreateCalendar(UI* ui)
{
	UICreatePanel(ui, &calendar_dlg);
}
