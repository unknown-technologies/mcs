#ifndef __FONT_H__
#define __FONT_H__

#include <mcs/types.h>
#include <mcs/tex.h>

typedef struct {
	float	posX;
	float	posY;
	float	width;
	float	height;
	float	offX;
	float	baseline;
	float	advance;
	float	adjust;
} GXGlyph;

typedef struct {
	u16	charcnt;
	u16	_pad;
	float	height;
	float	maxheight;
	float	maxbaseline;
	u8	charmap[256];
	GXGlyph	glyphs[1];
} FNT;

typedef struct {
	GLuint	metrics;
	GLuint	charmap;
	GXTexture tex;
	FNT*	font;
} GXFont;

void	GXInitFont(void);

BOOL	GXCreateFont(GXFont* font, const char* fontfile);
void	GXDestroyFont(GXFont* font);
float	GXGetFontHeight(GXFont* font, float size);
float	GXGetFontLineHeight(GXFont* font, float size);
float	GXGetTextWidth(GXFont* font, float size, const char* text);
float	GXGetTextWidthAlt(GXFont* font, float size, const char* text);
float	GXDrawText(GXFont* font, float x, float y, float scale,
		const float color[4], const char* text);
float	GXDrawTextAlt(GXFont* font, float x, float y, float scale,
		const float color[4], const char* text);

void	GXInitMicrofont(void);
void	GXDrawMicroText(unsigned int x, unsigned int y,
		const float color[4], const char* text);

/* internal subroutines */
float	GXiGetTextWidth(GXFont* font, BOOL alt, float size,
		const char* text);
float	GXiDrawText(GXFont* font, BOOL alt, float x, float y, float size,
		const float color[4], const char* text);

#endif
