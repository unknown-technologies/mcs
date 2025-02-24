#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include <mcs.h>

const float color_white[4]       = { 1.00, 1.00, 1.00, 1.00 };
const float color_gray[4]        = { 0.50, 0.50, 0.50, 1.00 };
const float color_red[4]         = { 1.00, 0.00, 0.00, 1.00 };
const float color_green[4]       = { 0.00, 1.00, 0.00, 1.00 };
const float color_yellow[4]      = { 1.00, 1.00, 0.00, 1.00 };
const float color_pink[4]        = { 1.00, 0.00, 1.00, 1.00 };
const float color_orange[4]      = { 1.00, 0.50, 0.00, 1.00 };
const float color_blue[4]        = { 0.00, 0.00, 1.00, 1.00 };
const float color_cyan[4]        = { 0.00, 1.00, 1.00, 1.00 };
const float color_blue_dark0[4]  = { 0.00, 0.06, 0.12, 1.00 };
const float color_blue_dark1[4]  = { 0.00, 0.25, 0.50, 1.00 };
const float color_blue_bright[4] = { 0.00, 0.50, 1.00, 1.00 };

extern const char FNT_deface[];

void UIInit(UI* ui, MCS* mcs)
{
	memset(ui, 0, sizeof(UI));

	ui->mcs = mcs;
	ui->current_panel = -1;
	ui->delete_pending = FALSE;
	ui->width = mcs->width;
	ui->height = mcs->height;

	GXFont* deface = &ui->deface;

	GXCreateFont(deface, FNT_deface);

	float width = GXGetTextWidth(deface, 192.0, "ALRM  ===> EDIT ")
		+ GXGetTextWidthAlt(deface, 192.0, "00:0000.00.0000");
	ui->status_size = (float) mcs->width / width * 192.0;
}

void UIDestroy(UI* ui)
{
	GXDestroyFont(&ui->deface);
}

UIPanel* UICreatePanel(UI* ui, UIPanelDefinition* proto)
{
	if(ui->current_panel + 1 >= UI_MAX_PANELS) {
		return NULL;
	}

	UIPanel* panel = &ui->panels[++ui->current_panel];
	memset(panel, 0, sizeof(UIPanel));

	panel->proto = proto;
	panel->ui = ui;
	panel->flags = UI_FLAG_ACTIVE;

	if(proto->init) {
		proto->init(panel);
	}

	return panel;
}

void UIDestroyPanel(UIPanel* panel)
{
	/* NULL is a valid argument, ignore it */
	if(!panel) {
		return;
	}

	/* this panel is already destroyed ... this is a programming bug,
	 * but let's ignore it anyway */
	if(panel->flags & UI_FLAG_DESTROY) {
		return;
	}

	if(panel->proto->destroy) {
		panel->proto->destroy(panel);
	}

	panel->flags |= UI_FLAG_DESTROY;

	panel->ui->delete_pending = TRUE;
}

#define UI_FLAGS_DESTROY	(UI_FLAG_DESTROY | UI_FLAG_ACTIVE)
BOOL UICleanup(UI* ui)
{
	if(!ui->delete_pending) {
		return FALSE;
	}

	BOOL cleaned = FALSE;

	/* delete programs if necessary */
	for(int i = 0; i <= ui->current_panel; i++) {
		UIPanel* panel = &ui->panels[i];

		if((panel->flags & UI_FLAGS_DESTROY) == UI_FLAGS_DESTROY) {
			/* UIDeleteProgram(panel); */
			panel->flags = 0;
			cleaned = TRUE;
		}
	}

	/* compact panel storage and remove deleted panels */
	int top = -1;
	for(int i = 0, wr = 0; i <= ui->current_panel; i++) {
		if(ui->panels[i].flags & UI_FLAG_ACTIVE) {
			if(i != wr) {
				ui->panels[wr] = ui->panels[i];
			}
			top = wr;
			wr++;
		}
	}

	ui->current_panel = top;
	ui->delete_pending = FALSE;

	return cleaned;
}

void UIProcess(UI* ui)
{
	MCS* mcs = ui->mcs;
	UIPanel* panel = UICurrentPanel(ui);

	if(!panel) {
		UICleanup(ui);
		return;
	}

	if(!(panel->flags & UI_FLAG_FULLSCN)) {
		GXFont* deface = &ui->deface;
		float size = ui->status_size;
		float pos_y = GXGetFontHeight(deface, size);
		float pos_x = GXGetTextWidthAlt(deface, size, "00:00:00");
		float touch_x = pos_x;
		if(panel->title) {
			char buf[64];
			sprintf(buf, "[%s]", panel->title);
			float width = GXGetTextWidth(deface, size, buf);
			touch_x = ui->width - width;
		}

		for(unsigned int i = 0; i < MTGetSlotCount(&mcs->mt); i++) {
			MT_SLOT* slot = MTGetSlot(&mcs->mt, i);
			if(!slot || !slot->active || !slot->fresh) {
				continue;
			}

			if(slot->x >= touch_x && slot->y <= pos_y) {
				UIDestroyPanel(panel);
				UICleanup(ui);
				return;
			}
		}
	}

	if(panel->proto->process) {
		panel->proto->process(panel);
	}

	UICleanup(ui);
}

void UIProcessInput(UIPanel* self,
		void (*callback)(UIPanel*, MT_SLOT*, void*),
		void* attachment)
{
	MCS* mcs = UIGetMCS(self);
	UI* ui = UIGet(self);
	BOOL fullscreen = self->flags & UI_FLAG_FULLSCN;

	float pos_y = 0;
	if(!fullscreen) {
		pos_y = GXGetFontHeight(&ui->deface, ui->status_size);
	}

	for(unsigned int i = 0; i < MTGetSlotCount(&mcs->mt); i++) {
		MT_SLOT* slot = MTGetSlot(&mcs->mt, i);
		if(!slot || !slot->active) {
			continue;
		} else if(fullscreen) {
			callback(self, slot, attachment);
		} else if(slot->y > pos_y) {
			MT_SLOT adjusted = *slot;
			adjusted.y -= pos_y;
			callback(self, &adjusted, attachment);
		}
	}
}

void UIDraw(UI* ui)
{
	UIPanel* panel = UICurrentPanel(ui);
	if(!panel) {
		return;
	}

	if(panel->flags & UI_FLAG_FULLSCN) {
		GXSetView(0, 0, ui->width, ui->height);
	} else {
		GXFont* deface = &ui->deface;

		float size = ui->status_size;
		float pos_y = GXGetFontHeight(deface, size);

		time_t t;
		time(&t);
		struct tm tm = *localtime(&t);
		char buf[64];
		sprintf(buf, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min,
				tm.tm_sec);

		GXSetView(0, 0, ui->width, ui->height);
		GXDrawTextAlt(deface, 0, 0, size, color_white, buf);

		if(panel->title) {
			sprintf(buf, "[%s]", panel->title);
			float width = GXGetTextWidth(deface, size, buf);
			float pos_x = ui->width - width;
			GXDrawText(deface, pos_x, 0, size, color_white,
					buf);
		}

		GXSetView(0, 0, ui->width, ui->height - pos_y);
	}

	if(panel->proto->draw) {
		panel->proto->draw(panel);
	}
}
