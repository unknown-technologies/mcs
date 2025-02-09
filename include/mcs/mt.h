#ifndef __MT_H__
#define __MT_H__

#include <mcs/types.h>

typedef struct {
	int		id;
	int		x;
	int		y;
	int		dx;
	int		dy;
	int		new_id;
	int		new_x;
	int		new_y;
	BOOL		active;
	BOOL		new_active;
	BOOL		fresh;
} MT_SLOT;

typedef struct {
	int		fd;

	int		width;
	int		height;
	int		axis_x_min;
	int		axis_y_min;

	int		slot_cnt;
	int		slot_min;
	MT_SLOT*	slots;

	MT_SLOT*	slot;
} MT;

BOOL	MTInit(MT* mt);
void	MTClose(MT* mt);
void	MTPoll(MT* mt);

MT_SLOT* MTGetSlot(MT* mt, int id);

static inline int MTGetSlotCount(MT* mt)
{
	return mt->slot_cnt;
}

#endif
