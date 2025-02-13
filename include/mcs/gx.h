#ifndef __GX_H__
#define __GX_H__

#ifdef NDEBUG
#define GL_ERROR()
#else
#define GL_ERROR()	GXCheckError(__FILE__, __LINE__)
#endif

void	GXInit(void);
void	GXSetView(int x, int y, int width, int height);
int	GXGetWidth(void);
int	GXGetHeight(void);

void	GXRenderQuad(void);
void	GXRenderTexQuad(const float pos_size[4], const float color[4],
		BOOL sdf);

void	GXCheckError(const char* filename, unsigned int line);

#endif
