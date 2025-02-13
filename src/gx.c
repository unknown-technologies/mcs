#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

#include <mcs.h>

static const float quad_vtx[] = {
	-1.0f, -1.0f,  0.0f,
	 1.0f, -1.0f,  0.0f,
	 1.0f,  1.0f,  0.0f,

	 1.0f,  1.0f,  0.0f,
	-1.0f,  1.0f,  0.0f,
	-1.0f, -1.0f,  0.0f
};

static GLuint quad_vbo;
static GLuint quad_vao;

static GLuint quad_shader;
static GLuint quad_screensz;
static GLuint quad_pos;
static GLuint quad_tex;
static GLuint quad_color;
static GLuint quad_sdf;

extern const char quad_vert[];
extern const char quad_frag[];

static int screen_width = 0;
static int screen_height = 0;

#define	QUAD_VTX_CNT	(sizeof(quad_vtx) / (sizeof(*quad_vtx) * 3))

void GXCheckError(const char* filename, unsigned int line)
{
	GLenum error = glGetError();
	switch(error) {
		case GL_NO_ERROR:
			break;
		case GL_INVALID_ENUM:
			printf("%s:%u: Error: GL_INVALID_ENUM\n",
					filename, line);
			break;
		case GL_INVALID_VALUE:
			printf("%s:%u: Error: GL_INVALID_VALUE\n",
					filename, line);
			break;
		case GL_INVALID_OPERATION:
			printf("%s:%u: Error: GL_INVALID_OPERATION\n",
					filename, line);
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			printf("%s:%u: Error: GL_INVALID_FRAMEBUFFER_"
					"OPERATION\n", filename, line);
			break;
		case GL_OUT_OF_MEMORY:
			printf("%s:%u: Error: GL_OUT_OF_MEMORY\n",
					filename, line);
			exit(1);
			break;
		case GL_STACK_UNDERFLOW:
			printf("%s:%u: Error: GL_STACK_UNDERFLOW\n",
					filename, line);
			break;
		case GL_STACK_OVERFLOW:
			printf("%s:%u: Error: GL_STACK_OVERFLOW\n",
					filename, line);
			break;
		default:
			printf("%s:%u: Unknown error 0x%X\n", filename,
					line, error);
	}
}

void GXInit(void)
{
	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);

	GLuint loc = 0;

	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vtx), quad_vtx,
			GL_STATIC_DRAW);

	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GL_ERROR();

	GXInitMicrofont();
	GXInitFont();

	/* basic shader to render a quad (plain or sdf) */
	if(!GXCreateShader(&quad_shader, quad_vert, quad_frag)) {
		printf("Failed to compile quad shader\n");
		exit(1);
	}

	quad_screensz = GXGetShaderUniform(&quad_shader, "screen_size");
	quad_pos = GXGetShaderUniform(&quad_shader, "quad");
	quad_tex = GXGetShaderUniform(&quad_shader, "tex");
	quad_color = GXGetShaderUniform(&quad_shader, "quadcolor");
	quad_sdf = GXGetShaderUniform(&quad_shader, "sdf");
}

void GXSetView(int x, int y, int width, int height)
{
	screen_width = width;
	screen_height = height;

	glViewport(x, y, width, height);
}

int GXGetWidth(void)
{
	return screen_width;
}

int GXGetHeight(void)
{
	return screen_height;
}

void GXRenderQuad(void)
{
	GL_ERROR();

	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, QUAD_VTX_CNT);
}

void GXRenderTexQuad(const float pos_size[4], const float color[4],
		BOOL sdf)
{
	glUseProgram(quad_shader);
	glUniform1i(quad_tex, 0);
	glUniform1i(quad_sdf, sdf);
	glUniform2f(quad_screensz, screen_width, screen_height);
	if(color) {
		glUniform4fv(quad_color, 1, color);
	}
	glUniform4fv(quad_pos, 1, pos_size);

	GXRenderQuad();
}
