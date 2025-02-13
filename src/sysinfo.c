#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

#include <mcs.h>

#define	SOFTWARE_VERSION	"r" REVISION "~" COMMIT

static const char* about_text_1 = "Build Date: \n"
	"Build Time: \n"
	"Version: \n"
	"\n"
	"Total memory: \n"
	"Free memory: \n"
	"Load: \n"
	"Cores: \n"
	"Uptime: ";

static const char* about_text_2 = BUILDDATE "\n"
	BUILDTIME "\n"
	SOFTWARE_VERSION;

void UIInitSysinfo(UIPanel* self)
{
	UISetTitle(self, "System Info");
}

void UIDrawSysinfo(UIPanel* self)
{
	UI* ui = UIGet(self);

	float size = 32;
	float pos_x = 0;
	float pos_y = 0;

	float width = GXGetTextWidth(&ui->deface, size, about_text_1);

	GXDrawText(&ui->deface, pos_x, pos_y, size, color_gray,
			about_text_1);
	GXDrawText(&ui->deface, pos_x + width, pos_y, size, color_gray,
			about_text_2);

	pos_y = GXGetFontLineHeight(&ui->deface, size) * 4;

	struct sysinfo info;
	sysinfo(&info);

	long uptime = info.uptime;
	int uptime_s = uptime % 60; uptime /= 60;
	int uptime_m = uptime % 60; uptime /= 60;
	int uptime_h = uptime % 24; uptime /= 24;
	int uptime_d = uptime;

	float load_scale = 1.0f / (1 << SI_LOAD_SHIFT);
	char buf[256];
	sprintf(buf, "%luMB\n"
			"%luMB\n"
			"%.2f %.2f %.2f\n"
			"%d\n"
			"%d days, %02d:%02d:%02d",
			info.totalram * (size_t) info.mem_unit / (1024 * 1024),
			info.freeram * (size_t) info.mem_unit / (1024 * 1024),
			info.loads[0] * load_scale,
			info.loads[1] * load_scale,
			info.loads[2] * load_scale,
			get_nprocs(),
			uptime_d, uptime_h, uptime_m, uptime_s);
	GXDrawText(&ui->deface, pos_x + width, pos_y, size, color_gray, buf);
}

static UIPanelDefinition sysinfo_dlg = {
	.init = UIInitSysinfo,
	.destroy = NULL,
	.process = NULL,
	.draw = UIDrawSysinfo
};

void UICreateSysinfo(UI* ui)
{
	UICreatePanel(ui, &sysinfo_dlg);
}
