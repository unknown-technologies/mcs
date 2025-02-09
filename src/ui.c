#include <stdlib.h>
#include <string.h>
#include <mcs.h>

void UIInit(UI* ui, MCS* mcs)
{
	memset(ui, 0, sizeof(UI));

	ui->mcs = mcs;
	ui->current_panel = -1;
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

void UIProcess(UI* ui)
{
	UIPanel* panel = UICurrentPanel(ui);
	if(panel && panel->proto->process) {
		panel->proto->process(panel);
	}
}

void UIDraw(UI* ui)
{
	UIPanel* panel = UICurrentPanel(ui);
	if(panel && panel->proto->draw) {
		panel->proto->draw(panel);
	}
}
