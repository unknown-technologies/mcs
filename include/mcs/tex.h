#ifndef __GXTEXTURE_H__
#define __GXTEXTURE_H__

#define	TXTR_FMT_RGBA8	0
#define	TXTR_FMT_RGB8	1
#define	TXTR_FMT_I8	2
#define	TXTR_FMT_A8	3

typedef struct {
	u16	width;
	u16	height;
	u8	data[1];
} TXTR;

typedef struct {
	u16	width;
	u16	height;
	GLuint	tex;
} GXTexture;

void	GXCreateTexture(GXTexture* tex, const TXTR* data);
void	GXDestroyTexture(GXTexture* tex);
void	GXUseTexture(GXTexture* tex);

static inline int GXGetTextureWidth(GXTexture* tex)
{
	return tex->width;
}

static inline int GXGetTextureHeight(GXTexture* tex)
{
	return tex->height;
}

#endif
