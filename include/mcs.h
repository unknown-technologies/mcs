#ifndef __MCS_H__
#define __MCS_H__

#ifdef DEBUG
#define	MCS_ALLOW_EXIT
#endif

#include <mcs/types.h>
#include <mcs/shader.h>
#include <mcs/gx.h>
#include <mcs/font.h>
#include <mcs/tex.h>
#include <mcs/mt.h>
#include <mcs/dsp.h>
#include <mcs/ai.h>
#include <mcs/snd.h>
#include <mcs/cal.h>
#include <mcs/os.h>
#include <mcs/cfg.h>
#include <mcs/kb.h>
#include <mcs/cfg.h>
#include <mcs/sha256.h>
#include <mcs/aes256.h>
#include <time.h>

typedef struct MCS MCS;

#include <mcs/ui.h>
#include <mcs/panels.h>

typedef struct {
	int		hour;
	int		minute;
	int		second;
} OSTime;

typedef struct {
	OSTime		alarm;
	time_t		next_alarm;
	BOOL		enabled;
	BOOL		triggered;
	BOOL		editing;
} MCSClock;

struct MCS {
	int		width;
	int		height;
	MT		mt;
	UI		ui;
	MCSClock	clock;
	CFG		config;
#ifdef MCS_ALLOW_EXIT
	BOOL		exit;
#endif
};

void	MCSInit(MCS* mcs, int width, int height);
void	MCSFree(MCS* mcs);

void	MCSProcess(MCS* mcs);
void	MCSRender(MCS* mcs);

#endif
