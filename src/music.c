#include <mcs.h>

void UIInitMusic(UIPanel* self)
{
	UISetTitle(self, "Music");
}

void UIDestroyMusic(UIPanel* self)
{
	(void) self;
}

void UIProcessMusic(UIPanel* self)
{
	(void) self;
}

void UIDrawMusic(UIPanel* self)
{
	(void) self;
}

static UIPanelDefinition music_dlg = {
	.init = UIInitMusic,
	.destroy = UIDestroyMusic,
	.process = UIProcessMusic,
	.draw = UIDrawMusic
};

void UICreateMusic(UI* ui)
{
	UICreatePanel(ui, &music_dlg);
}
