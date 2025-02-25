#ifndef __CFG_H__
#define __CFG_H__

#include <stddef.h>

#define	CFG_AUTH_SIZE	32

typedef struct {
	const char*	timezone;
	const char*	server;
	const char*	username;
	const u8*	auth;
} CFG;

void	CFGLoadDefaults(CFG* cfg);
void	CFGRead(CFG* cfg);
void	CFGWrite(CFG* cfg);
void	CFGDestroy(CFG* cfg);

BOOL	CFGSetTimeZone(CFG* cfg, const char* tz);

/* internal subroutines */
void	CFGiParse(CFG* cfg, void* data, size_t size);
void	CFGiProcess(CFG* cfg);

#endif
