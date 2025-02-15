#include <mcs.h>

void UIInitSettings(UIPanel* self)
{
	UISetTitle(self, "Settings");
}

void UIDestroySettings(UIPanel* self)
{
	(void) self;
}

void UIProcessSettings(UIPanel* self)
{
	(void) self;
}

void UIDrawSettings(UIPanel* self)
{
	(void) self;
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
