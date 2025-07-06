#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <mcs.h>

#define	ICONS_PER_ROW	5
#define	ICONS_PER_COL	2

typedef struct {
	const char*	name;
	const TXTR*	texture;
	BOOL		sdf;
	void		(*callback)(UI*);
} TILE;

typedef struct {
	const char*	name;
	void		(*callback)(UI*);
	GXTexture	texture;
	BOOL		sdf;
	float		icon_x;
	float		icon_y;
	float		icon_w;
	float		icon_h;
	float		text_x;
	float		text_y;
} DesktopTile;

typedef struct {
	DesktopTile*	tiles;
	int		count;
	float		text_size;
	float		icon_size;
} Desktop;

extern const TXTR TEX_gear;
extern const TXTR TEX_terminal;

static const TILE tile_definition[] = {
	{ .name = "TIMER",	.texture = &TEX_gear,
		.sdf = TRUE,	.callback = UICreateTimer },
	{ .name = "CALENDAR",	.texture = &TEX_gear,
		.sdf = TRUE,	.callback = UICreateCalendar },
	{ .name = "MUSIC",	.texture = &TEX_gear,
		.sdf = TRUE,	.callback = UICreateMusic },
	{ .name = "LIGHT",	.texture = &TEX_gear,
		.sdf = TRUE,	.callback = UICreateLight },
	{ .name = "NEWS",	.texture = &TEX_gear,
		.sdf = TRUE,	.callback = UICreateNews },
	{ .name = "TERMINAL",	.texture = &TEX_terminal,
		.sdf = FALSE,	.callback = NULL },
	{ .name = "SYSINFO",	.texture = &TEX_gear,
		.sdf = TRUE,	.callback = UICreateSysinfo },
	{ .name = "SETTINGS",	.texture = &TEX_gear,
		.sdf = TRUE,	.callback = UICreateSettings },
	{ .name = "HELP",	.texture = &TEX_gear,
		.sdf = TRUE,	.callback = NULL },
	{ .name = NULL,		.texture = NULL, .callback = NULL }
};

void UIInitDesktop(UIPanel* self)
{
	UISetTitle(self, "Desktop");

	int count = 0;
	for(const TILE* tile = tile_definition; tile->name; tile++) {
		count++;
	}

	Desktop* desktop = (Desktop*) malloc(sizeof(Desktop));
	memset(desktop, 0, sizeof(Desktop));
	desktop->count = count;
	desktop->tiles = (DesktopTile*) malloc(count * sizeof(DesktopTile));
	memset(desktop->tiles, 0, count * sizeof(DesktopTile));

	GXFont* deface = &UIGet(self)->deface;
	desktop->text_size = 32;
	desktop->icon_size = 128;

	float cell_width = UIGetWidth(self) / ICONS_PER_ROW;
	float cell_height = UIGetHeight(self) / ICONS_PER_COL;

	float cell_content = desktop->icon_size +
		GXGetFontHeight(deface, desktop->text_size);
	float pos_x = cell_width / 2.0f;
	float pos_y = (cell_height - cell_content) / 2.0;

	int cells = 0;
	for(int i = 0; i < count; i++) {
		DesktopTile* tile = &desktop->tiles[i];
		tile->name = tile_definition[i].name;
		tile->callback = tile_definition[i].callback;
		tile->sdf = tile_definition[i].sdf;

		GXCreateTexture(&tile->texture,
				tile_definition[i].texture);

		float text_width = GXGetTextWidth(deface,
				desktop->text_size, tile->name);

		/* layout everything */
		float iconsize = fmaxf(GXGetTextureWidth(&tile->texture),
				GXGetTextureHeight(&tile->texture));
		float scale = desktop->icon_size / iconsize;

		tile->icon_w = GXGetTextureWidth(&tile->texture) * scale;
		tile->icon_h = GXGetTextureHeight(&tile->texture) * scale;
		tile->icon_x = pos_x - tile->icon_w / 2.0f;
		tile->icon_y = pos_y;

		tile->text_x = pos_x - text_width / 2.0f;
		tile->text_y = pos_y + desktop->icon_size;

		pos_x += cell_width;
		cells++;
		if(cells >= ICONS_PER_ROW) {
			cells = 0;
			pos_x = cell_width / 2.0f;
			pos_y += cell_height;
		}
	}

	UISetUserData(self, desktop);
}

void UIDestroyDesktop(UIPanel* self)
{
	Desktop* desktop = (Desktop*) UIGetUserData(self);
	for(int i = 0; i < desktop->count; i++) {
		GXDestroyTexture(&desktop->tiles[i].texture);
	}
	free(desktop->tiles);
	free(desktop);
}

void UIProcessDesktopInput(UIPanel* self, MT_SLOT* slot, void* attachment)
{
	BOOL* processed = (BOOL*) attachment;

	if(slot->fresh && !*processed) {
		Desktop* desktop = (Desktop*) UIGetUserData(self);
		for(int i = 0; i < desktop->count; i++) {
			DesktopTile* tile = &desktop->tiles[i];
			float px1 = tile->icon_x;
			float py1 = tile->icon_y;
			float px2 = px1 + tile->icon_w;
			float py2 = py1 + tile->icon_h;

			if(slot->x >= px1 && slot->x <= px2 &&
					slot->y >= py1 && slot->y <= py2) {
				if(tile->callback) {
					tile->callback(UIGet(self));
				}
				*processed = TRUE;
				return;
			}
		}
	}
}

void UIProcessDesktop(UIPanel* self)
{
	BOOL processed = FALSE;

	UIProcessInput(self, UIProcessDesktopInput, (void*) &processed);
}

void UIDrawDesktop(UIPanel* self)
{
	UI* ui = UIGet(self);

	GXFont* deface = &ui->deface;
	Desktop* desktop = (Desktop*) UIGetUserData(self);
	for(int i = 0; i < desktop->count; i++) {
		DesktopTile* tile = &desktop->tiles[i];

		float pos_size[4] = {
			tile->icon_x, tile->icon_y,
			tile->icon_w, tile->icon_h
		};

		glActiveTexture(GL_TEXTURE0);
		GXUseTexture(&tile->texture);
		GXRenderTexQuad(pos_size, color_white, tile->sdf);

		GXDrawText(deface, tile->text_x, tile->text_y,
				desktop->text_size, color_gray, tile->name);
	}
}

static UIPanelDefinition desktop_dlg = {
	.init = UIInitDesktop,
	.destroy = UIDestroyDesktop,
	.process = UIProcessDesktop,
	.draw = UIDrawDesktop
};

void UICreateDesktop(UI* ui)
{
	UICreatePanel(ui, &desktop_dlg);
}
