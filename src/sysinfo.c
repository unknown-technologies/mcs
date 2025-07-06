#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netdb.h>

#include <mcs.h>

#define	SOFTWARE_VERSION	"r" REVISION "~" COMMIT

static const char* about_text_1 = "Build Time: \n"
	"Version: \n"
	"\n"
	"Total memory: \n"
	"Free memory: \n"
	"Load: \n"
	"Cores: \n"
	"Uptime: ";

static const char* about_text_2 = BUILDDATE " " BUILDTIME "\n"
	SOFTWARE_VERSION;

typedef struct {
	struct ifaddrs*	addrs;
} Sysinfo;

void UIInitSysinfo(UIPanel* self)
{
	Sysinfo* info = (Sysinfo*) malloc(sizeof(Sysinfo));
	memset(info, 0, sizeof(Sysinfo));
	UISetUserData(self, info);

	UISetTitle(self, "System Info");

	if(getifaddrs(&info->addrs)) {
		printf("getifaddrs failed: %s\n", strerror(errno));
	}
}

void UIDestroySysinfo(UIPanel* self)
{
	Sysinfo* info = (Sysinfo*) UIGetUserData(self);
	if(info->addrs) {
		freeifaddrs(info->addrs);
	}
	free(info);
}

#ifdef MCS_ALLOW_EXIT
static void UIProcessSysinfoInput(UIPanel* self, MT_SLOT* slot, void* data)
{
	if(slot->fresh) {
		MCS* mcs = UIGetMCS(self);
		mcs->exit = TRUE;
	}
}

void UIProcessSysinfo(UIPanel* self)
{
	UIProcessInput(self, UIProcessSysinfoInput, NULL);
}
#endif

void UIDrawSysinfo(UIPanel* self)
{
	UI* ui = UIGet(self);
	GXFont* deface = &ui->deface;
	Sysinfo* this = (Sysinfo*) UIGetUserData(self);

	float size = 32;
	float pos_x = 0;
	float pos_y = 0;

	float width = GXGetTextWidth(deface, size, about_text_1);

	GXDrawText(deface, pos_x, pos_y, size, color_gray, about_text_1);
	GXDrawText(deface, pos_x + width, pos_y, size, color_gray,
			about_text_2);

	pos_y = GXGetFontLineHeight(deface, size) * 3;

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
	GXDrawText(deface, pos_x + width, pos_y, size, color_gray, buf);

	pos_y += GXGetFontLineHeight(deface, size) * 6;

	for(struct ifaddrs* ifa = this->addrs; ifa; ifa = ifa->ifa_next) {
		/* no address => uninteresting for us */
		if(!ifa->ifa_addr) {
			continue;
		}

		/* loopback interface => uninteresting for us */
		if(ifa->ifa_name && !strcmp(ifa->ifa_name, "lo")) {
			continue;
		}

		/* we only care about IPv4/IPv6 */
		int family = ifa->ifa_addr->sa_family;
		if(family != AF_INET && family != AF_INET6) {
			continue;
		}

		GXDrawText(deface, pos_x, pos_y, size, color_gray,
				ifa->ifa_name);

		const float* color = color_gray;
		int err = getnameinfo(ifa->ifa_addr, (family == AF_INET) ?
				sizeof(struct sockaddr_in) :
				sizeof(struct sockaddr_in6), buf,
				sizeof(buf), NULL, 0, NI_NUMERICHOST);
		if(err) {
			sprintf(buf, "getnameinfo(): %s",
					gai_strerror(err));
			color = color_orange;
		}

		GXDrawText(deface, pos_x + width, pos_y, size, color, buf);
		pos_y += GXGetFontLineHeight(deface, size);
	}
}

static UIPanelDefinition sysinfo_dlg = {
	.init = UIInitSysinfo,
	.destroy = UIDestroySysinfo,
#ifdef MCS_ALLOW_EXIT
	.process = UIProcessSysinfo,
#else
	.process = NULL,
#endif
	.draw = UIDrawSysinfo
};

void UICreateSysinfo(UI* ui)
{
	UICreatePanel(ui, &sysinfo_dlg);
}
