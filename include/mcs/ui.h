#ifndef __UI_H__
#define __UI_H__

#include <stdlib.h>
#include <mcs.h>

#define	UI_MAX_PANELS	16

#define	UI_FLAG_ACTIVE	1

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
	int		flags;
};

struct UI {
	UIPanel		panels[UI_MAX_PANELS];
	int		current_panel;
	MCS*		mcs;
};

void	UIInit(UI* ui, MCS* mcs);
UIPanel* UICreatePanel(UI* ui, UIPanelDefinition* panel);
void	UIProcess(UI* ui);
void	UIDraw(UI* ui);

static inline MCS* UIGetMCS(UIPanel* panel)
{
	return panel->ui->mcs;
}

static inline UI* UIGet(UIPanel* panel)
{
	return panel->ui;
}

static inline UIPanel* UICurrentPanel(UI* ui)
{
	if(ui->current_panel == -1) {
		return NULL;
	} else {
		return &ui->panels[ui->current_panel];
	}
}

#endif
