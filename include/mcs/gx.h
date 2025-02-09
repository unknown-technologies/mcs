#ifndef __GX_H__
#define __GX_H__

#ifdef NDEBUG
#define GL_ERROR()
#else
#define GL_ERROR()	GXCheckError(__FILE__, __LINE__)
#endif

#define	TXTR_FMT_RGBA8	0
#define	TXTR_FMT_RGB8	1
#define	TXTR_FMT_I8	2
#define	TXTR_FMT_A8	3

typedef struct {
	u16	width;
	u16	height;
	u8	data[1];
} TXTR;

void	GXInit(void);
void	GXRenderQuad(void);

void	GXCheckError(const char* filename, unsigned int line);

#endif
