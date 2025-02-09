#ifndef __AI_H__
#define __AI_H__

#include <mcs/types.h>

BOOL	AIInit(void (*callback)(s16*, unsigned int));
void	AIDestroy(void);

#endif
