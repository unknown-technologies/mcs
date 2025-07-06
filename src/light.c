#include <stdio.h>
#include <string.h>

#include <mcs.h>

#define	FADER_WIDTH	64
#define	CHANNEL_FADERS	4
#define	CHANNEL_COLORS	4

static const float* channel_colors[CHANNEL_COLORS] = {
	color_red, color_green, color_blue, color_white
};

typedef struct {
	float		text_size;
	UIFader		master_fader;
	UIFader		channel_faders[CHANNEL_FADERS];
} Light;

void UIInitLight(UIPanel* self)
{
	DMX* dmx = &UIGetMCS(self)->dmx;
	GXFont* deface = &UIGet(self)->deface;

	UISetTitle(self, "Light");

	Light* light = (Light*) malloc(sizeof(Light));
	UISetUserData(self, light);

	float width = UIGetWidth(self);
	float height = UIGetHeight(self);

	float maxwidth = GXGetTextWidthAlt(deface, 256.0f, "000");
	light->text_size = FADER_WIDTH / maxwidth * 256.0f;

	UICreateFader(&light->master_fader, width - FADER_WIDTH,
			light->text_size / 2.0f,
			height - 2.0f * light->text_size, FADER_WIDTH,
			UIFADER_ORIENTATION_VERTICAL,
			0, 255, dmx->master);

	float px = FADER_WIDTH;
	for(unsigned int i = 0; i < CHANNEL_FADERS; i++) {
		/* TODO: fix this */
		int value = dmx->fixtures[0].params[i];
		UICreateFader(&light->channel_faders[i], px,
				light->text_size / 2.0f,
				height - 2.0f * light->text_size,
				FADER_WIDTH,
				UIFADER_ORIENTATION_VERTICAL,
				0, 255, value);

		px += FADER_WIDTH * 1.1;
	}
}

void UIDestroyLight(UIPanel* self)
{
	Light* light = (Light*) UIGetUserData(self);

	UIDestroyFader(&light->master_fader);

	free(light);
}

void UIProcessLightInput(UIPanel* self, MT_SLOT* slot, void* attachment)
{
	Light* light = (Light*) UIGetUserData(self);

	UIProcessFader(&light->master_fader, slot);

	for(unsigned int i = 0; i < CHANNEL_FADERS; i++) {
		UIProcessFader(&light->channel_faders[i], slot);
	}
}

void UIProcessLight(UIPanel* self)
{
	UIProcessInput(self, UIProcessLightInput, NULL);

	/* transfer faders to DMX stuff */
	/* TODO: fix this */

	Light* light = (Light*) UIGetUserData(self);
	DMX* dmx = &UIGetMCS(self)->dmx;
	UIFader* faders = light->channel_faders;

	dmx->master = UIGetFaderValue(&light->master_fader);

	for(unsigned int i = 0; i < CHANNEL_FADERS; i++) {
		if(i > DMX_MAXCH) {
			break;
		}

		int value = UIGetFaderValue(&faders[i]);
		for(unsigned int j = 0; j < 3; j++) {
			dmx->fixtures[j].params[i] = value;
		}
	}
}

void UIDrawLight(UIPanel* self)
{
	Light* light = (Light*) UIGetUserData(self);
	GXFont* deface = &UIGet(self)->deface;

	UIFader* master_fader = &light->master_fader;
	UIDrawFader(master_fader);

	/* draw master fader label */
	float master_x = master_fader->x - FADER_WIDTH / 2.0f;
	float master_y = master_fader->y + master_fader->length;

	char buf[4];
	sprintf(buf, "%03d", UIGetFaderValue(master_fader));
	GXDrawTextAlt(deface, master_x, master_y, light->text_size,
			color_blue, buf);

	for(unsigned int i = 0; i < CHANNEL_FADERS; i++) {
		UIFader* fader = &light->channel_faders[i];
		const float* color = i < CHANNEL_COLORS ? channel_colors[i]
			: color_blue;

		UIDrawFader(fader);

		float pos_x = fader->x - FADER_WIDTH / 2.0f;
		float pos_y = fader->y + fader->length;

		char buf[4];
		sprintf(buf, "%03d", UIGetFaderValue(fader));
		GXDrawTextAlt(deface, pos_x, pos_y, light->text_size, color,
				buf);
	}
}

static UIPanelDefinition light_dlg = {
	.init = UIInitLight,
	.destroy = UIDestroyLight,
	.process = UIProcessLight,
	.draw = UIDrawLight
};

void UICreateLight(UI* ui)
{
	UICreatePanel(ui, &light_dlg);
}

