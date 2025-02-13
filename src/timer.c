#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <mcs.h>

typedef struct {
	float		size;
	float		pos_y;
	float		pos_x_h;
	float		pos_x_m;
	float		pos_x_s;
	float		button_inc;
	float		button_dec;
	float		button_width;
	float		text_height;

	unsigned int	remaining;
	unsigned long	end_time;
	BOOL		active;
	BOOL		triggered;
} Timer;

static unsigned long get_time(void)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

void UIInitTimer(UIPanel* self)
{
	UISetTitle(self, "Timer");

	Timer* timer = (Timer*) malloc(sizeof(Timer));
	memset(timer, 0, sizeof(Timer));
	UISetUserData(self, timer);

	GXFont* deface = &UIGet(self)->deface;

	float width = GXGetTextWidthAlt(deface, 192.0, "00:00:00");
	timer->size = UIGetWidth(self) / width * 192.0;
	timer->text_height = GXGetFontHeight(deface, timer->size);
	timer->pos_y = (UIGetHeight(self) - timer->text_height) / 2.0f;

	float offset = GXGetFontHeight(deface, timer->size) * 0.7;
	timer->button_inc = timer->pos_y - offset;
	timer->button_dec = timer->pos_y + offset;

	timer->button_width = GXGetTextWidthAlt(deface, timer->size, "00");
	timer->pos_x_h = 0;
	timer->pos_x_m = timer->pos_x_h +
		GXGetTextWidthAlt(deface, timer->size, "00:");
	timer->pos_x_s = timer->pos_x_m +
		GXGetTextWidthAlt(deface, timer->size, "00:");
}

void UIDestroyTimer(UIPanel* self)
{
	Timer* timer = (Timer*) UIGetUserData(self);

	if(timer->triggered) {
		SNDStopAlarm();
	}

	free(timer);
}

static inline unsigned long set_timer(unsigned int h, unsigned int m,
		unsigned int s)
{
	return ((h * 60 + m) * 60) + s;
}

void UIProcessTimerInput(UIPanel* self, MT_SLOT* slot, void* attachment)
{
	Timer* timer = (Timer*) UIGetUserData(self);

	(void) attachment;

	unsigned int t = timer->remaining;
	unsigned int timer_s = t % 60;	t /= 60;
	unsigned int timer_m = t % 60;	t /= 60;
	unsigned int timer_h = t;

	if(slot->fresh) {
		/* the time itself */
		if(slot->y >= timer->pos_y && slot->y <=
				timer->pos_y + timer->text_height) {
			timer->active = !timer->active;
			if(timer->active) {
				timer->end_time = get_time() +
					timer->remaining * 1000;
			}
			timer->triggered = FALSE;
			SNDStopAlarm();
			return;
		}

		if(timer->active) {
			return;
		}

		/* hour field */
		if(slot->x <= (timer->pos_x_h + timer->button_width)) {
			if(slot->y < timer->pos_y && timer_h < 99) {
				timer_h++;
			} else if(slot->y > (timer->pos_y +
						timer->text_height) &&
					timer_h > 0) {
				timer_h--;
			}
		}

		/* minute field */
		if(slot->x >= timer->pos_x_m && slot->x <=
				(timer->pos_x_m + timer->button_width)) {
			if(slot->y < timer->pos_y) {
				timer_m = (timer_m + 1) % 60;
			} else if(slot->y > (timer->pos_y +
						timer->text_height)) {
				if(timer_m > 0) {
					timer_m--;
				} else {
					timer_m = 59;
				}
			}
		}

		/* second field */
		if(slot->x >= timer->pos_x_s) {
			if(slot->y < timer->pos_y) {
				timer_s = (timer_s + 1) % 60;
			} else if(slot->y > (timer->pos_y +
						timer->text_height)) {
				if(timer_s > 0) {
					timer_s--;
				} else {
					timer_s = 59;
				}
			}
		}
	}

	timer->remaining = set_timer(timer_h, timer_m, timer_s);
}

void UIProcessTimer(UIPanel* self)
{
	Timer* timer = (Timer*) UIGetUserData(self);

	UIProcessInput(self, UIProcessTimerInput, NULL);

	if(timer->active && !timer->triggered) {
		unsigned long now = get_time();
		if(now >= timer->end_time) {
			/* finished */
			timer->triggered = TRUE;
			timer->remaining = 0;
			SNDPlayAlarm();
		} else {
			unsigned long dt = timer->end_time - now;
			timer->remaining = (dt + 999) / 1000;
		}
	}
}

void UIDrawTimer(UIPanel* self)
{
	UI* ui = UIGet(self);
	Timer* timer = (Timer*) UIGetUserData(self);

	GXFont* deface = &ui->deface;

	unsigned int t = timer->remaining;
	unsigned int timer_s = t % 60;	t /= 60;
	unsigned int timer_m = t % 60;	t /= 60;
	unsigned int timer_h = t;

	char buf[16];
	sprintf(buf, "%02d:%02d:%02d", timer_h, timer_m, timer_s);

	const float* color = color_white;
	if(timer->triggered) {
		unsigned long now = get_time();
		unsigned long dt = now - timer->end_time;
		if((dt % 1000) < 500) {
			color = color_red;
		} else {
			color = color_green;
		}
	} else if(timer->active) {
		color = color_green;
	}

	GXDrawTextAlt(deface, 0, timer->pos_y, timer->size, color, buf);

	if(!timer->active) {
		GXDrawTextAlt(deface, timer->pos_x_h, timer->button_inc,
				timer->size, color_red, "++");
		GXDrawTextAlt(deface, timer->pos_x_m, timer->button_inc,
				timer->size, color_red, "++");
		GXDrawTextAlt(deface, timer->pos_x_s, timer->button_inc,
				timer->size, color_red, "++");

		GXDrawTextAlt(deface, timer->pos_x_h, timer->button_dec,
				timer->size, color_red, "--");
		GXDrawTextAlt(deface, timer->pos_x_m, timer->button_dec,
				timer->size, color_red, "--");
		GXDrawTextAlt(deface, timer->pos_x_s, timer->button_dec,
				timer->size, color_red, "--");
	}
}

static UIPanelDefinition timer_dlg = {
	.init = UIInitTimer,
	.destroy = UIDestroyTimer,
	.process = UIProcessTimer,
	.draw = UIDrawTimer
};

void UICreateTimer(UI* ui)
{
	UICreatePanel(ui, &timer_dlg);
}
