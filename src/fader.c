#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <mcs.h>

#define	FADER_TRACK_WIDTH	2
#define	FADER_BUTTON_WIDTH	8

extern const char fader_vert[];
extern const char fader_frag[];

static unsigned int refcnt = 0;
static GXShader shader;
static GLuint shader_screensz;
static GLuint shader_quad;
static GLuint shader_color;

void UICreateFader(UIFader* fader, float x, float y, float length,
		float width, unsigned int orientation, int minval,
		int maxval, int value)
{
	memset(fader, 0, sizeof(UIFader));

	fader->x = x;
	fader->y = y;
	fader->length = length;
	fader->width = width;
	fader->orientation = orientation;
	fader->minval = minval;
	fader->maxval = maxval;
	fader->value = value;

	if(fader->orientation == UIFADER_ORIENTATION_VERTICAL) {
		fader->track[0] = fader->x - FADER_TRACK_WIDTH / 2.0f;
		fader->track[1] = fader->y;
		fader->track[2] = FADER_TRACK_WIDTH;
		fader->track[3] = fader->length;

		fader->min_x = fader->x - fader->width / 2.0f;
		fader->min_y = fader->y - FADER_BUTTON_WIDTH / 2.0f;
		fader->max_x = fader->min_x + fader->width;
		fader->max_y = fader->min_y + fader->length
			+ FADER_BUTTON_WIDTH;
	} else {
		fader->track[0] = fader->x;
		fader->track[1] = fader->y - FADER_TRACK_WIDTH / 2.0f;
		fader->track[2] = fader->length;
		fader->track[3] = FADER_TRACK_WIDTH;

		fader->min_x = fader->x - FADER_BUTTON_WIDTH / 2.0f;
		fader->min_y = fader->y - fader->width / 2.0f;
		fader->max_x = fader->min_x + fader->length
			+ FADER_BUTTON_WIDTH;
		fader->max_y = fader->min_y + fader->width;
	}

	if(refcnt == 0) {
		if(!GXCreateShader(&shader, fader_vert, fader_frag)) {
			printf("Failed to compile fader shader\n");
			exit(1);
		}

		shader_screensz = GXGetShaderUniform(&shader, "screen_size");
		shader_quad = GXGetShaderUniform(&shader, "quad");
		shader_color = GXGetShaderUniform(&shader, "quadcolor");
	}
	refcnt++;
}

void UIDestroyFader(UIFader* fader)
{
	refcnt--;
	(void) fader;

	if(refcnt == 0) {
		GXDestroyShader(&shader);
	}
}

void UIDrawFader(UIFader* fader)
{
	float button[4];

	if(fader->orientation == UIFADER_ORIENTATION_VERTICAL) {
		float pos = 1.0f - (fader->value - fader->minval)
			/ ((double) fader->maxval - fader->minval);
		button[0] = fader->x - fader->width / 2.0f;
		button[1] = fader->y + pos * fader->length
			- FADER_BUTTON_WIDTH / 2.0f;
		button[2] = fader->width;
		button[3] = FADER_BUTTON_WIDTH;
	} else {
		float pos = (fader->value - fader->minval)
			/ ((double) fader->maxval - fader->minval);
		button[0] = fader->x + pos * fader->length
			- FADER_BUTTON_WIDTH / 2.0f;
		button[1] = fader->y - fader->width / 2.0f;
		button[2] = FADER_BUTTON_WIDTH;
		button[3] = fader->width;
	}

	GXUseShader(&shader);
	glUniform2f(shader_screensz, GXGetWidth(), GXGetHeight());

	/* track */
	glUniform4fv(shader_quad, 1, fader->track);
	glUniform4fv(shader_color, 1, color_blue);
	GXRenderQuad();

	/* button */
	glUniform4fv(shader_quad, 1, button);
	glUniform4fv(shader_color, 1, color_cyan);
	GXRenderQuad();
}

void UIProcessFader(UIFader* fader, MT_SLOT* slot)
{
	if(slot->x >= fader->min_x && slot->x <= fader->max_x &&
			slot->y >= fader->min_y && slot->y <= fader->max_y) {
		/* touch point is on the fader */
		if(fader->orientation == UIFADER_ORIENTATION_VERTICAL) {
			float point = slot->y - fader->y;
			float pos = 1.0f - point / fader->length;
			if(pos > 1.0f) {
				pos = 1.0f;
			} else if(pos < 0.0f) {
				pos = 0.0f;
			}
			float val = pos * (fader->maxval - fader->minval)
				+ fader->minval;
			fader->value = (int) roundf(val);
		} else {
			float point = slot->x - fader->x;
			float pos = point / fader->length;
			if(pos > 1.0f) {
				pos = 1.0f;
			} else if(pos < 0.0f) {
				pos = 0.0f;
			}
			float val = pos * (fader->maxval - fader->minval)
				+ fader->minval;
			fader->value = (int) roundf(val);
		}
	}
}
