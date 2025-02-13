#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <mcs.h>

extern const char font_vert[];
extern const char font_frag[];

static GLuint font_shader;
static GLuint font_screensz;
static GLuint font_scale;
static GLuint font_tex;
static GLuint font_charmap;
static GLuint font_metrics;
static GLuint font_pos;
static GLuint font_color;
static GLuint font_glyph;
static GLuint font_maxbaseline;

void GXInitFont(void)
{
	if(!GXCreateShader(&font_shader, font_vert, font_frag)) {
		printf("Failed to compile font shader\n");
		exit(1);
	}

	font_screensz = GXGetShaderUniform(&font_shader, "screen_size");
	font_scale = GXGetShaderUniform(&font_shader, "scale");
	font_tex = GXGetShaderUniform(&font_shader, "font");
	font_charmap = GXGetShaderUniform(&font_shader, "charmap");
	font_metrics = GXGetShaderUniform(&font_shader, "metrics");
	font_pos = GXGetShaderUniform(&font_shader, "textpos");
	font_color = GXGetShaderUniform(&font_shader, "textcolor");
	font_glyph = GXGetShaderUniform(&font_shader, "glyph");
	font_maxbaseline = GXGetShaderUniform(&font_shader, "maxbaseline");
	GL_ERROR();
}

BOOL GXCreateFont(GXFont* font, const char* fontfile)
{

	FNT* fnt = (FNT*) fontfile;
	TXTR* txtr = (TXTR*) &fnt->glyphs[fnt->charcnt];

	font->font = fnt;
	GXCreateTexture(&font->tex, txtr);

	glGenTextures(1, &font->charmap);
	glBindTexture(GL_TEXTURE_2D, font->charmap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 256, 1, 0, GL_RED_INTEGER,
			GL_UNSIGNED_BYTE, (GLvoid*) fnt->charmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GL_ERROR();

	glGenTextures(1, &font->metrics);
	glBindTexture(GL_TEXTURE_2D, font->metrics);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 2, fnt->charcnt, 0,
			GL_RGBA, GL_FLOAT, (GLvoid*) fnt->glyphs);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	GL_ERROR();

	return TRUE;
}

void GXDestroyFont(GXFont* font)
{
	GXDestroyTexture(&font->tex);
	glDeleteTextures(1, &font->charmap);
	glDeleteTextures(1, &font->metrics);
}

float GXGetFontHeight(GXFont* font, float size)
{
	FNT* fnt = font->font;
	float scale = size / (float) fnt->height;

	/* TODO: is this a good idea? */
	return fnt->maxheight * scale;
}

float GXGetFontLineHeight(GXFont* font, float size)
{
	(void) font;
	return size;
}

float GXGetTextWidth(GXFont* font, float size, const char* text)
{
	return GXiGetTextWidth(font, FALSE, size, text);
}

float GXGetTextWidthAlt(GXFont* font, float size, const char* text)
{
	return GXiGetTextWidth(font, TRUE, size, text);
}

float GXiGetTextWidth(GXFont* font, BOOL alt, float size, const char* text)
{
	FNT* fnt = font->font;

	float scale = size / (float) font->font->height;

	float px = 0;
	float py = 0;
	float width = 0;

	for(const char* c = text; *c; c++) {
		GXGlyph* glyph = NULL;
		u8 idx = fnt->charmap[(u8) *c];
		if(idx != 0xFF) {
			glyph = &fnt->glyphs[idx];
		}
		if(*c == '\n') {
			if(px > width) {
				width = px;
			}
			px = 0;
			py += fnt->height * scale;
			continue;
		} else if(!glyph) {
			continue;
		}

		px += glyph->advance * scale;
		if(alt) {
			px += glyph->adjust * scale;
		}
	}

	if(px > width) {
		width = px;
	}

	return width;
}

void GXDrawText(GXFont* font, float x, float y, float size,
		const float color[4], const char* text)
{
	GXiDrawText(font, FALSE, x, y, size, color, text);
}

void GXDrawTextAlt(GXFont* font, float x, float y, float size,
		const float color[4], const char* text)
{
	GXiDrawText(font, TRUE, x, y, size, color, text);
}

void GXiDrawText(GXFont* font, BOOL alt, float x, float y, float size,
		const float color[4], const char* text)
{
	FNT* fnt = font->font;

	float scale = size / (float) font->font->height;
	glUseProgram(font_shader);
	glUniform1f(font_scale, scale);
	glUniform1f(font_maxbaseline, font->font->maxbaseline);
	glUniform1i(font_tex, 0);
	glUniform1i(font_charmap, 1);
	glUniform1i(font_metrics, 2);
	glUniform2f(font_screensz, GXGetWidth(), GXGetHeight());
	glUniform4fv(font_color, 1, color);

	glActiveTexture(GL_TEXTURE0);
	GXUseTexture(&font->tex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, font->charmap);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, font->metrics);

	float px = x;
	float py = y;
	for(const char* c = text; *c; c++) {
		GXGlyph* glyph = NULL;
		u8 idx = fnt->charmap[(u8) *c];
		if(idx != 0xFF) {
			glyph = &fnt->glyphs[idx];
		}
		if(*c == '\n') {
			px = x;
			py += fnt->height * scale;
			continue;
		} else if(!glyph) {
			continue;
		}

		float offX = alt ? (glyph->adjust / 2.0f) * scale : 0.0f;
		glUniform2f(font_pos, px + offX, py);
		glUniform1ui(font_glyph, (u8) *c);

		GXRenderQuad();

		px += glyph->advance * scale;
		if(alt) {
			px += glyph->adjust * scale;
		}
	}
}
