#ifndef __WIDGETS_H__
#define __WIDGETS_H__

#include <mcs/ui.h>
#include <mcs/mt.h>

#define	UIFADER_ORIENTATION_HORIZONTAL	0
#define	UIFADER_ORIENTATION_VERTICAL	1

typedef struct {
	float		x;
	float		y;
	float		length;
	float		width;
	int		value;
	int		minval;
	int		maxval;
	unsigned int	orientation;

	/* precomputed track geometry */
	float		track[4];

	/* dimensions for fast touch processing */
	float		min_x;
	float		max_x;
	float		min_y;
	float		max_y;
} UIFader;

void	UICreateFader(UIFader* fader, float x, float y, float length,
		float width, unsigned int orientation, int minval,
		int maxval, int value);
void	UIDestroyFader(UIFader* fader);
void	UIDrawFader(UIFader* fader);
void	UIProcessFader(UIFader* fader, MT_SLOT* slot);

static inline int UIGetFaderValue(UIFader* fader)
{
	return fader->value;
}

static inline void UISetFaderValue(UIFader* fader, int value)
{
	fader->value = value;
}

static inline void UIGetFaderPosition(UIFader* fader, float* x, float* y)
{
	*x = fader->x;
	*y = fader->y;
}

#endif
