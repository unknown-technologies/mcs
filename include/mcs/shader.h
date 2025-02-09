#ifndef __GXShader_H__
#define __GXShader_H__

#include <GL/gl.h>

#include <mcs/types.h>

typedef GLuint	GXShader;
typedef	GLuint	GXUniform;

BOOL	GXCreateShader(GXShader* shader, const char* vs_src,
		const char* fs_src);
void	GXDestroyShader(GXShader* shader);
GXUniform GXGetShaderUniform(GXShader* shader, const char* name);
void	GXUseShader(GXShader* shader);

#endif
