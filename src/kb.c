#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <GL/gl.h>

#include <mcs.h>

#define	EDGE_QUADS	3
#define	RADIUS		7.0f
#define	THICKNESS	2.0f
#define	PADDING		2.0f
#define	GLYPH_PADDING	4.0f

#define	TYPE_IGNORE	0
#define	TYPE_OUTLINE	1
#define	TYPE_FILL	2
#define	TYPE_GLYPH	3

extern const char keyboard_vert[];
extern const char keyboard_frag[];

static KBVTX* KBCreateKey(KBVTX* vtx, GXFont* font, float fontscale,
		float maxascend, float maxheight, float key_size,
		float x, float y, unsigned int id, unsigned int code)
{
	const float key_width = key_size;
	const float key_height = key_size;

	const float inc = M_PI / (2.0 * EDGE_QUADS);
	for(unsigned int i = 0; i < EDGE_QUADS; i++) {
		float phi0 = i * inc;
		float phi1 = (i + 1) * inc;

		/* 00 = point 0, outer; 01 = point 0, inner
		 * 10 = point 1, outer; 11 = point 1, inner */
		float px00 = cosf(phi0) * RADIUS;
		float px01 = cosf(phi0) * (RADIUS - THICKNESS);
		float px10 = cosf(phi1) * RADIUS;
		float px11 = cosf(phi1) * (RADIUS - THICKNESS);
		float py00 = sinf(phi0) * RADIUS;
		float py01 = sinf(phi0) * (RADIUS - THICKNESS);
		float py10 = sinf(phi1) * RADIUS;
		float py11 = sinf(phi1) * (RADIUS - THICKNESS);

		/* top right */
		vtx->x = x + key_width - RADIUS + px00;
		vtx->y = y + key_height - RADIUS + py00;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + key_width - RADIUS + px01;
		vtx->y = y + key_height - RADIUS + py01;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + key_width - RADIUS + px10;
		vtx->y = y + key_height - RADIUS + py10;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + key_width - RADIUS + px10;
		vtx->y = y + key_height - RADIUS + py10;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + key_width - RADIUS + px11;
		vtx->y = y + key_height - RADIUS + py11;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + key_width - RADIUS + px01;
		vtx->y = y + key_height - RADIUS + py01;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		/* top left */
		vtx->x = x + RADIUS - px00;
		vtx->y = y + key_height - RADIUS + py00;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + RADIUS - px01;
		vtx->y = y + key_height - RADIUS + py01;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + RADIUS - px10;
		vtx->y = y + key_height - RADIUS + py10;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + RADIUS - px10;
		vtx->y = y + key_height - RADIUS + py10;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + RADIUS - px11;
		vtx->y = y + key_height - RADIUS + py11;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + RADIUS - px01;
		vtx->y = y + key_height - RADIUS + py01;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		/* bottom right */
		vtx->x = x + key_width - RADIUS + px00;
		vtx->y = y + RADIUS - py00;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + key_width - RADIUS + px01;
		vtx->y = y + RADIUS - py01;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + key_width - RADIUS + px10;
		vtx->y = y + RADIUS - py10;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + key_width - RADIUS + px10;
		vtx->y = y + RADIUS - py10;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + key_width - RADIUS + px11;
		vtx->y = y + RADIUS - py11;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + key_width - RADIUS + px01;
		vtx->y = y + RADIUS - py01;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		/* bottom left */
		vtx->x = x + RADIUS - px00;
		vtx->y = y + RADIUS - py00;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + RADIUS - px01;
		vtx->y = y + RADIUS - py01;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + RADIUS - px10;
		vtx->y = y + RADIUS - py10;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + RADIUS - px10;
		vtx->y = y + RADIUS - py10;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + RADIUS - px11;
		vtx->y = y + RADIUS - py11;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		vtx->x = x + RADIUS - px01;
		vtx->y = y + RADIUS - py01;
		vtx->id = id;
		vtx->type = TYPE_OUTLINE;
		vtx++;

		/* fill: top right */
		vtx->x = x + key_width - RADIUS + px01;
		vtx->y = y + key_height - RADIUS + py01;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;

		vtx->x = x + key_width - RADIUS + px11;
		vtx->y = y + key_height - RADIUS + py11;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;

		vtx->x = x + key_width - RADIUS;
		vtx->y = y + key_height - RADIUS;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;

		/* fill: top left */
		vtx->x = x + RADIUS - px01;
		vtx->y = y + key_height - RADIUS + py01;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;

		vtx->x = x + RADIUS - px11;
		vtx->y = y + key_height - RADIUS + py11;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;

		vtx->x = x + RADIUS;
		vtx->y = y + key_height - RADIUS;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;

		/* fill: bottom right */
		vtx->x = x + key_width - RADIUS + px01;
		vtx->y = y + RADIUS - py01;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;

		vtx->x = x + key_width - RADIUS + px11;
		vtx->y = y + RADIUS - py11;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;

		vtx->x = x + key_width - RADIUS;
		vtx->y = y + RADIUS;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;

		/* fill: bottom left */
		vtx->x = x + RADIUS - px01;
		vtx->y = y + RADIUS - py01;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;

		vtx->x = x + RADIUS - px11;
		vtx->y = y + RADIUS - py11;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;

		vtx->x = x + RADIUS;
		vtx->y = y + RADIUS;
		vtx->id = id;
		vtx->type = TYPE_FILL;
		vtx++;
	}

	/* top */
	vtx->x = x + RADIUS;
	vtx->y = y;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y + THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y + THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + RADIUS;
	vtx->y = y + THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + RADIUS;
	vtx->y = y;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	/* bottom */
	vtx->x = x + RADIUS;
	vtx->y = y + key_height - THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y + key_height - THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y + key_height;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y + key_height;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + RADIUS;
	vtx->y = y + key_height;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + RADIUS;
	vtx->y = y + key_height - THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	/* left */
	vtx->x = x;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + THICKNESS;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + THICKNESS;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + THICKNESS;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	/* right */
	vtx->x = x + key_width - THICKNESS;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + key_width;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + key_width;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + key_width;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + key_width - THICKNESS;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	vtx->x = x + key_width - THICKNESS;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_OUTLINE;
	vtx++;

	/* fill: top */
	vtx->x = x + RADIUS;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + RADIUS;
	vtx->y = y + THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y + THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y + THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + RADIUS;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	/* fill: bottom */
	vtx->x = x + RADIUS;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + RADIUS;
	vtx->y = y + key_height - THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y + key_height - THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y + key_height - THICKNESS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + key_width - RADIUS;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + RADIUS;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	/* fill: center */
	vtx->x = x + THICKNESS;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + key_width - THICKNESS;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + key_width - THICKNESS;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + key_width - THICKNESS;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + THICKNESS;
	vtx->y = y + key_height - RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	vtx->x = x + THICKNESS;
	vtx->y = y + RADIUS;
	vtx->id = id;
	vtx->type = TYPE_FILL;
	vtx++;

	/* glyph */
	FNT* fnt = font->font;

	GXGlyph* glyph = NULL;
	if(code <= 255) {
		u8 idx = fnt->charmap[code];
		if(idx != 0xFF) {
			glyph = &fnt->glyphs[idx];
		}
	}

	if(glyph) {
		float posX = glyph->posX;
		float posY = glyph->posY;
		float width = glyph->width;
		float height = glyph->height;
		float offY = maxascend - glyph->baseline;

		float pad_x = (key_size - width * fontscale) / 2.0f;
		float pad_y = (key_size - maxheight * fontscale)
			/ 2.0f;
		pad_y += offY * fontscale;

		vtx->x = x + pad_x;
		vtx->y = y + pad_y;
		vtx->u = posX / font->tex.width;
		vtx->v = posY / font->tex.height;
		vtx->id = id;
		vtx->type = TYPE_GLYPH;
		vtx++;

		vtx->x = x + pad_x + width * fontscale;
		vtx->y = y + pad_y;
		vtx->u = (posX + width) / font->tex.width;
		vtx->v = posY / font->tex.height;
		vtx->id = id;
		vtx->type = TYPE_GLYPH;
		vtx++;

		vtx->x = x + pad_x + width * fontscale;
		vtx->y = y + pad_y + height * fontscale;
		vtx->u = (posX + width) / font->tex.width;
		vtx->v = (posY + height) / font->tex.height;
		vtx->id = id;
		vtx->type = TYPE_GLYPH;
		vtx++;

		vtx->x = x + pad_x + width * fontscale;
		vtx->y = y + pad_y + height * fontscale;
		vtx->u = (posX + width) / font->tex.width;
		vtx->v = (posY + height) / font->tex.height;
		vtx->id = id;
		vtx->type = TYPE_GLYPH;
		vtx++;

		vtx->x = x + pad_x;
		vtx->y = y + pad_y + height * fontscale;
		vtx->u = posX / font->tex.width;
		vtx->v = (posY + height) / font->tex.height;
		vtx->id = id;
		vtx->type = TYPE_GLYPH;
		vtx++;

		vtx->x = x + pad_x;
		vtx->y = y + pad_y;
		vtx->u = posX / font->tex.width;
		vtx->v = posY / font->tex.height;
		vtx->id = id;
		vtx->type = TYPE_GLYPH;
		vtx++;
	} else {
		vtx->type = TYPE_IGNORE;
		vtx++;
		vtx->type = TYPE_IGNORE;
		vtx++;
		vtx->type = TYPE_IGNORE;
		vtx++;

		vtx->type = TYPE_IGNORE;
		vtx++;
		vtx->type = TYPE_IGNORE;
		vtx++;
		vtx->type = TYPE_IGNORE;
		vtx++;
	}

	return vtx;
}

void KBInit(KB* kb, GXFont* font, const KBLayout* layout, int width,
		int height)
{
	memset(kb, 0, sizeof(KB));
	kb->font = &font->tex;
	kb->layout = layout;
	kb->top = FALSE;

	unsigned int key_cnt = layout->row_cnt * layout->col_cnt;
	const unsigned int quad_cnt = 4 * EDGE_QUADS + 4 + 3 + 1;
	const unsigned int tri_cnt = 4 * EDGE_QUADS;
	const unsigned int vertex_cnt = key_cnt *
		(tri_cnt * 3 + quad_cnt * 6);

	KBVTX* vertices = (KBVTX*) malloc(layout->layer_cnt *
			vertex_cnt * sizeof(KBVTX));
	kb->vertices = vertices;
	kb->vtx_cnt = vertex_cnt;
	memset(vertices, 0, vertex_cnt * sizeof(KBVTX));

	const float cell_size = width / (float) layout->col_cnt;
	const float key_size = cell_size - 2 * PADDING;
	const float key_glyphsize = key_size - 2 * GLYPH_PADDING;

	kb->cell_size = cell_size;

	/* compute glyph sizes */
	FNT* fnt = font->font;
	float maxascend = 0;
	float maxdescend = 0;
	float maxwidth = 0;
	for(unsigned int i = 0; i < layout->layer_cnt; i++) {
		const KBLayer* layer = &layout->layers[i];
		for(unsigned int rowid = 0; rowid < layout->row_cnt;
				rowid++) {
			const KBRow* row = &layer->rows[rowid];
			for(unsigned int colid = 0; colid < row->count;
					colid++) {
				const KBKey* key = &row->keys[colid];
				unsigned int code = key->code;
				if(code > 255) {
					continue;
				}

				u8 idx = fnt->charmap[code];
				if(idx == 0xFF) {
					continue;
				}

				GXGlyph* glyph = &fnt->glyphs[idx];

				if(glyph->width > maxwidth) {
					maxwidth = glyph->width;
				}

				float ascend = glyph->baseline;
				if(ascend > maxascend) {
					maxascend = ascend;
				}

				float descend = glyph->height -
					glyph->baseline;
				if(descend > maxdescend) {
					maxdescend = descend;
				}
			}
		}
	}

	float maxheight = maxascend + maxdescend;
	float maxsize = maxheight;

	if(maxwidth > maxsize) {
		maxsize = maxwidth;
	}

	float fontscale = key_glyphsize / fnt->height;

	KBVTX* vtx = vertices;
	for(unsigned int i = 0; i < layout->layer_cnt; i++) {
		const KBLayer* layer = &layout->layers[i];
		float px = PADDING;
		float py = 0;
		for(unsigned int rowid = 0; rowid < layout->row_cnt;
				rowid++) {
			const KBRow* row = &layer->rows[rowid];
			for(unsigned int colid = 0; colid < row->count;
					colid++) {
				const KBKey* key = &row->keys[colid];
				vtx = KBCreateKey(vtx, font, fontscale,
						maxascend, maxheight,
						key_size, px, py, key->id,
						key->code);
				px += cell_size;
			}
			px = PADDING;
			py += cell_size;
		}

		if(i == 0) {
			kb->height = py;
		}
	}

	if(vtx != &vertices[layout->layer_cnt * vertex_cnt]) {
		abort();
	}

	kb->vao = (GLuint*) malloc(layout->layer_cnt * sizeof(GLuint));
	kb->vbo = (GLuint*) malloc(layout->layer_cnt * sizeof(GLuint));

	glGenVertexArrays(layout->layer_cnt, kb->vao);
	glGenBuffers(layout->layer_cnt, kb->vbo);

	for(unsigned int i = 0; i < layout->layer_cnt; i++) {
		glBindVertexArray(kb->vao[i]);

		glBindBuffer(GL_ARRAY_BUFFER, kb->vbo[i]);
		glBufferData(GL_ARRAY_BUFFER, vertex_cnt * sizeof(KBVTX),
				&vertices[i * vertex_cnt], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
				sizeof(KBVTX),
				(const void*) offsetof(KBVTX, x));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
				sizeof(KBVTX),
				(const void*) offsetof(KBVTX, u));
		glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT,
				sizeof(KBVTX),
				(const void*) offsetof(KBVTX, id));
		glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT,
				sizeof(KBVTX),
				(const void*) offsetof(KBVTX, type));
	}

	GL_ERROR();

	if(!GXCreateShader(&kb->shader, keyboard_vert, keyboard_frag)) {
		printf("Failed to compile keyboard shader\n");
		exit(1);
	}

	kb->shader_font = GXGetShaderUniform(&kb->shader, "font");
	kb->shader_offset_y = GXGetShaderUniform(&kb->shader, "offsetY");
	kb->shader_screensz = GXGetShaderUniform(&kb->shader, "screen_size");
	kb->shader_keys_down = GXGetShaderUniform(&kb->shader, "keys_down");
}

void KBDestroy(KB* kb)
{
	GXDestroyShader(&kb->shader);
	glDeleteVertexArrays(kb->layout->layer_cnt, kb->vao);
	glDeleteBuffers(kb->layout->layer_cnt, kb->vbo);
	free(kb->vao);
	free(kb->vbo);
	free(kb->vertices);
}

void KBDraw(KB* kb)
{
	GL_ERROR();

	GXUseShader(&kb->shader);
	glUniform1i(kb->shader_font, 0);
	if(kb->top) {
		glUniform1f(kb->shader_offset_y, 0);
	} else {
		glUniform1f(kb->shader_offset_y,
				GXGetHeight() - kb->height);
	}
	glUniform2f(kb->shader_screensz, GXGetWidth(), GXGetHeight());
	glUniform1uiv(kb->shader_keys_down, 4, kb->keys_down);

	glActiveTexture(GL_TEXTURE0);
	GXUseTexture(kb->font);

	glBindVertexArray(kb->vao[kb->current_layer]);
	glDrawArrays(GL_TRIANGLES, 0, kb->vtx_cnt);
}

void KBProcess(KB* kb)
{
	memcpy(kb->last_down, kb->keys_down, sizeof(kb->keys_down));
	memset(kb->keys_down, 0, sizeof(kb->keys_down));

	/* find the currently selected layer: check for all layers if all
	 * modifiers are currently held. If yes: select it. Otherwise: use
	 * the layer with all-zero modifiers */
	BOOL match = FALSE;
	unsigned int longest = 0;
	unsigned int default_layer = 0;
	for(unsigned int i = 0; i < kb->layout->layer_cnt; i++) {
		const KBLayer* layer = &kb->layout->layers[i];
		BOOL all = TRUE;
		unsigned int count = 0;
		for(unsigned int j = 0; j < 2; j++) {
			unsigned int id = layer->modifiers[j];
			if(id == 0) {
				continue;
			}

			unsigned int idx = id / 32;
			unsigned int bit = 1 << (id % 32);
			if(kb->last_down[idx] & bit) {
				count++;
			} else {
				all = FALSE;
				break;
			}
		}

		if(all) {
			if(count == 0) {
				default_layer = i;
			} else if(longest < count) {
				longest = count;
				match = TRUE;
				kb->current_layer = i;
				break;
			}
		}
	}

	if(!match) {
		kb->current_layer = default_layer;
	}

	if(kb->current_layer >= kb->layout->layer_cnt) {
		printf("invalid layer\n");
		abort();
	}
}

void KBProcessInput(KB* kb, MT_SLOT* slot)
{
	float off_y = kb->top ? 0 : GXGetHeight() - kb->height;
	int colid = (int) (slot->x / kb->cell_size);
	int rowid = (int) ((slot->y - off_y) / kb->cell_size);

	/* ignore touch points outside of the keyboard area */
	if(kb->top && slot->y > kb->height) {
		return;
	} else if(!kb->top && slot->y < off_y) {
		return;
	}

	if(rowid < kb->layout->row_cnt) {
		const KBLayer* layer = &kb->layout->layers[kb->current_layer];
		const KBRow* row = &layer->rows[rowid];
		if(colid < row->count) {
			const KBKey* key = &row->keys[colid];
			if(key->id > (sizeof(kb->keys_down) * 8)) {
				return;
			}
			unsigned int idx = key->id / 32;
			unsigned int bit = 1 << (key->id % 32);
			if((kb->last_down[idx] & bit) || slot->fresh) {
				kb->keys_down[idx] |= bit;
			}
			if(slot->fresh && kb->callback) {
				kb->callback(key->code, kb->userdata);
			}
		}
	}
}
