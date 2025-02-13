#ifndef __UI_H__
#define __UI_H__

#include <stdlib.h>
#include <mcs.h>

#define	UI_MAX_PANELS	16

#define	UI_FLAG_ACTIVE	1
#define	UI_FLAG_DESTROY	2
#define	UI_FLAG_FULLSCN	4

typedef struct UIPanel UIPanel;
typedef struct UI UI;

typedef struct {
	void		(*init)(UIPanel* self);
	void		(*destroy)(UIPanel* self);
	void		(*process)(UIPanel* self);
	void		(*draw)(UIPanel* self);
} UIPanelDefinition;

struct UIPanel {
	const UIPanelDefinition* proto;
	UI*		ui;
	const char*	title;
	void*		userdata;
	int		flags;
};

struct UI {
	UIPanel		panels[UI_MAX_PANELS];
	int		current_panel;
	BOOL		delete_pending;
	int		width;
	int		height;
	float		status_size;
	MCS*		mcs;
	GXFont		deface;
};

extern const float color_white[4];
extern const float color_gray[4];
extern const float color_red[4];
extern const float color_green[4];
extern const float color_yellow[4];
extern const float color_pink[4];
extern const float color_orange[4];

void	UIInit(UI* ui, MCS* mcs);
void	UIDestroy(UI* ui);
UIPanel* UICreatePanel(UI* ui, UIPanelDefinition* panel);
void	UIDestroyPanel(UIPanel* panel);
void	UIProcess(UI* ui);
void	UIDraw(UI* ui);

void	UIProcessInput(UIPanel* self,
		void (*callback)(UIPanel*, MT_SLOT*, void*),
		void* attachment);

static inline MCS* UIGetMCS(UIPanel* panel)
{
	return panel->ui->mcs;
}

static inline UI* UIGet(UIPanel* panel)
{
	return panel->ui;
}

static inline void* UIGetUserData(UIPanel* panel)
{
	return panel->userdata;
}

static inline void UISetUserData(UIPanel* panel, void* data)
{
	panel->userdata = data;
}

static inline UIPanel* UICurrentPanel(UI* ui)
{
	if(ui->current_panel == -1) {
		return NULL;
	} else {
		return &ui->panels[ui->current_panel];
	}
}

static inline void UISetFullscreen(UIPanel* panel, BOOL fullscreen)
{
	if(fullscreen) {
		panel->flags |= UI_FLAG_FULLSCN;
	} else {
		panel->flags &= ~UI_FLAG_FULLSCN;
	}
}

static inline void UISetTitle(UIPanel* panel, const char* title)
{
	panel->title = title;
}

static inline int UIGetWidth(UIPanel* panel)
{
	return UIGet(panel)->width;
}

static inline int UIGetHeight(UIPanel* panel)
{
	if(panel->flags & UI_FLAG_FULLSCN) {
		return UIGet(panel)->height;
	} else {
		UI* ui = UIGet(panel);
		float pos_y = GXGetFontHeight(&ui->deface, ui->status_size);
		return ui->height - pos_y;
	}
}

#endif
