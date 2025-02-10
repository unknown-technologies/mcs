#ifndef __GX_H__
#define __GX_H__

#ifdef NDEBUG
#define GL_ERROR()
#else
#define GL_ERROR()	GXCheckError(__FILE__, __LINE__)
#endif

void	GXInit(void);
void	GXRenderQuad(void);

void	GXCheckError(const char* filename, unsigned int line);

#endif
