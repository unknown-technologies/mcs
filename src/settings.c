#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mcs.h>

#define	BUFSZ	64

typedef struct {
	KB		kb;
	char		buffer[BUFSZ];
	unsigned int	pos;
	unsigned int	current_field;
	float		size;
	float		field_x;
	BOOL		kb_visible;
	BOOL		next_kb_visible;
} Settings;

#define	FIELD_TZ	0
#define	FIELD_SERVER	1
#define	FIELD_USERNAME	2
#define	FIELD_PASSWORD	3
#define	FIELD_CNT	4

static const char* labels[FIELD_CNT] = {
	/* 00 */ "Timezone: ",
	/* 01 */ "Server: ",
	/* 02 */ "Username: ",
	/* 03 */ "Password: "
};

static void callback(unsigned int key, void* attachment)
{
	UIPanel* panel = (UIPanel*) attachment;
	CFG* cfg = &UIGetMCS(panel)->config;
	Settings* self = (Settings*) UIGetUserData(panel);

	if(key == 0x7F) {
		if(self->pos > 0) {
			self->buffer[--self->pos] = 0;
		}
	} else if(key == '\r') {
		self->next_kb_visible = FALSE;
		self->buffer[self->pos] = 0;
		const char* value = self->buffer;
		switch(self->current_field) {
			case FIELD_TZ:
				CFGSetTimeZone(cfg, value);
				break;
		}
	} else if(key >= 0x20 && key <= 0xFF && self->pos + 1 < BUFSZ) {
		self->buffer[self->pos++] = (char) key;
	}
}

void UIInitSettings(UIPanel* self)
{
	UI* ui = UIGet(self);

	Settings* settings = (Settings*) malloc(sizeof(Settings));
	memset(settings, 0, sizeof(Settings));
	UISetUserData(self, settings);

	KBInit(&settings->kb, &ui->deface, &keyboard_layout_qwerty,
			UIGetWidth(self), UIGetHeight(self));
	KBSetPosition(&settings->kb, KB_POSITION_BOTTOM);
	KBSetCallback(&settings->kb, callback);
	KBSetUserData(&settings->kb, self);

	settings->kb_visible = FALSE;

	GXFont* deface = &ui->deface;
	settings->size = ui->status_size;

	float maxwidth = 0;
	for(unsigned int i = 0; i < FIELD_CNT; i++) {
		float width = GXGetTextWidth(deface, settings->size,
				labels[i]);
		if(width > maxwidth) {
			maxwidth = width;
		}
	}
	settings->field_x = maxwidth;

	UISetTitle(self, "Settings");
}

void UIDestroySettings(UIPanel* self)
{
	MCS* mcs = UIGetMCS(self);
	Settings* settings = (Settings*) UIGetUserData(self);
	KBDestroy(&settings->kb);
	free(settings);

	CFGWrite(&mcs->config);
}

static void UIProcessSettingsInput(UIPanel* self, MT_SLOT* slot,
		void* attachment)
{
	Settings* settings = (Settings*) UIGetUserData(self);

	if(slot->fresh && slot->x >= settings->field_x) {
		/* now get the line */
		UI* ui = UIGet(self);
		float height = GXGetFontLineHeight(&ui->deface,
				settings->size);
		int line = (int) (slot->y / height);

		if(line >= 0 && line < 3) {
			CFG* cfg = &UIGetMCS(self)->config;
			settings->next_kb_visible = TRUE;
			settings->current_field = line;
			memset(settings->buffer, 0, BUFSZ);
			settings->pos = 0;
			const char* value = NULL;
			switch(line) {
				case FIELD_TZ:
					value = cfg->timezone;
					break;
				case FIELD_SERVER:
					value = cfg->server;
					break;
				case FIELD_USERNAME:
					value = cfg->username;
					break;
			}

			if(value) {
				strncpy(settings->buffer, value, BUFSZ - 1);
				settings->pos = strlen(settings->buffer);
			}

			return;
		}
	}

	if(settings->kb_visible) {
		KBProcessInput(&settings->kb, slot);
	}
}

void UIProcessSettings(UIPanel* self)
{
	Settings* settings = (Settings*) UIGetUserData(self);

	if(settings->kb_visible) {
		KBProcess(&settings->kb);
	}

	settings->next_kb_visible = settings->kb_visible;
	UIProcessInput(self, UIProcessSettingsInput, NULL);
	settings->kb_visible = settings->next_kb_visible;
}

void UIDrawSettings(UIPanel* self)
{
	Settings* settings = (Settings*) UIGetUserData(self);
	UI* ui = UIGet(self);
	MCS* mcs = UIGetMCS(self);
	CFG* cfg = &mcs->config;
	GXFont* deface = &ui->deface;
	float size = ui->status_size;
	float line_height = GXGetFontLineHeight(deface, size);

	const char* timezone = cfg->timezone ? cfg->timezone : "(unset)";
	const char* server   = cfg->server   ? cfg->server   : "(unset)";
	const char* username = cfg->username ? cfg->username : "(unset)";

	const float* color_timezone = color_blue_bright;
	const float* color_server   = color_blue_bright;
	const float* color_username = color_blue_bright;

	if(settings->kb_visible) {
		switch(settings->current_field) {
			case FIELD_TZ:
				timezone = settings->buffer;
				color_timezone = color_orange;
				break;
			case FIELD_SERVER:
				server = settings->buffer;
				color_server = color_orange;
				break;
			case FIELD_USERNAME:
				username = settings->buffer;
				color_username = color_orange;
				break;
		}
	}

	float x = settings->field_x;
	float y = 0;
	float px;
	float cursor_x = 0;
	float cursor_y = 0;
	GXDrawText(deface, 0, y, size, color_white, labels[FIELD_TZ]);
	px = GXDrawText(deface, x, y, size, color_timezone, timezone);
	if(settings->current_field == FIELD_TZ) {
		cursor_x = px;
		cursor_y = y;
	}
	y += line_height;

	GXDrawText(deface, 0, y, size, color_white, labels[FIELD_SERVER]);
	px = GXDrawText(deface, x, y, size, color_server, server);
	if(settings->current_field == FIELD_SERVER) {
		cursor_x = px;
		cursor_y = y;
	}
	y += line_height;

	GXDrawText(deface, 0, y, size, color_white, labels[FIELD_USERNAME]);
	px = GXDrawText(deface, x, y, size, color_username, username);
	if(settings->current_field == FIELD_USERNAME) {
		cursor_x = px;
		cursor_y = y;
	}
	y += line_height;

	if(settings->kb_visible) {
		GXDrawText(&ui->deface, cursor_x, cursor_y, size,
				color_blue, "|");
		KBDraw(&settings->kb);
	}
}

static UIPanelDefinition settings_dlg = {
	.init = UIInitSettings,
	.destroy = UIDestroySettings,
	.process = UIProcessSettings,
	.draw = UIDrawSettings
};

void UICreateSettings(UI* ui)
{
	UICreatePanel(ui, &settings_dlg);
}
