#include <stdio.h>
#include <stdlib.h>

#include <mcs.h>

#define	MICROFONT_WIDTH		9
#define	MICROFONT_HEIGHT	96

extern const unsigned char microfont_bits[96 * 9];

extern const char microfont_vert[];
extern const char microfont_frag[];

static GLuint microfont;
static GLuint microfont_shader;
static GLuint microfont_screensz;
static GLuint microfont_tex;
static GLuint microfont_pos;
static GLuint microfont_color;
static GLuint microfont_glyph;

void GXInitMicrofont(void)
{
	glGenTextures(1, &microfont);
	glBindTexture(GL_TEXTURE_2D, microfont);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, MICROFONT_WIDTH,
			MICROFONT_HEIGHT, 0, GL_RED_INTEGER,
			GL_UNSIGNED_BYTE, microfont_bits);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GL_ERROR();

	if(!GXCreateShader(&microfont_shader, microfont_vert,
				microfont_frag)) {
		printf("Failed to compile microfont shader\n");
		exit(1);
	}

	microfont_screensz = GXGetShaderUniform(&microfont_shader,
			"screen_size");
	microfont_tex = GXGetShaderUniform(&microfont_shader, "font");
	microfont_pos = GXGetShaderUniform(&microfont_shader, "textpos");
	microfont_color = GXGetShaderUniform(&microfont_shader, "textcolor");
	microfont_glyph = GXGetShaderUniform(&microfont_shader, "glyph");
	GL_ERROR();
}

void GXDrawMicroText(unsigned int x, unsigned int y, const float color[4],
		const char* text)
{
	glUseProgram(microfont_shader);
	glUniform1i(microfont_tex, 0);
	glUniform2f(microfont_screensz, GXGetWidth(), GXGetHeight());
	glUniform4fv(microfont_color, 1, color);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, microfont);

	unsigned int px = x;
	unsigned int py = y;
	for(const char* c = text; *c; c++) {
		if(*c == '\n') {
			px = x;
			py += 10;
			continue;
		} else if(*c == ' ') {
			px += 6;
			continue;
		}

		glUniform2f(microfont_pos, px, py);
		glUniform1ui(microfont_glyph, *c - 0x20);

		GXRenderQuad();

		px += 6;
	}
}
