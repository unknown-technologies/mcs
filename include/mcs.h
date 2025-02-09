#ifndef __MCS_H__
#define __MCS_H__

#include <mcs/types.h>
#include <mcs/shader.h>
#include <mcs/gx.h>
#include <mcs/font.h>
#include <mcs/mt.h>
#include <mcs/dsp.h>
#include <mcs/ai.h>
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
	GXFont		deface;
	UI		ui;
	MCSClock	clock;
};

void	MCSInit(MCS* mcs, int width, int height);
void	MCSFree(MCS* mcs);

void	MCSProcess(MCS* mcs);
void	MCSRender(MCS* mcs);

#endif
