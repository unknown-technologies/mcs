#include <mcs.h>

void UIInitNews(UIPanel* self)
{
	UISetTitle(self, "News");
}

void UIDestroyNews(UIPanel* self)
{
	(void) self;
}

void UIProcessNews(UIPanel* self)
{
	(void) self;
}

void UIDrawNews(UIPanel* self)
{
	(void) self;
}

static UIPanelDefinition news_dlg = {
	.init = UIInitNews,
	.destroy = UIDestroyNews,
	.process = UIProcessNews,
	.draw = UIDrawNews
};

void UICreateNews(UI* ui)
{
	UICreatePanel(ui, &news_dlg);
}
