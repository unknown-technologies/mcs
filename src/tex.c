#include <mcs.h>

void GXCreateTexture(GXTexture* tex, const TXTR* txtr)
{
	u8 fmt = (txtr->width >> 14) & 0x03;

	GL_ERROR();

	glGenTextures(1, &tex->tex);
	glBindTexture(GL_TEXTURE_2D, tex->tex);

	switch(fmt) {
		case TXTR_FMT_A8:
		case TXTR_FMT_I8:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
					txtr->width & 0x3FFF,
					txtr->height & 0x3FFF, 0,
					GL_RED, GL_UNSIGNED_BYTE,
					txtr->data);
			break;
		case TXTR_FMT_RGB8:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,
					txtr->width & 0x3FFF,
					txtr->height & 0x3FFF, 0,
					GL_RGB, GL_UNSIGNED_BYTE,
					txtr->data);
			break;
		case TXTR_FMT_RGBA8:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
					txtr->width & 0x3FFF,
					txtr->height & 0x3FFF, 0,
					GL_RGBA, GL_UNSIGNED_BYTE,
					(GLvoid*) txtr->data);
			break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GL_ERROR();
}

void GXDestroyTexture(GXTexture* tex)
{
	glDeleteTextures(1, &tex->tex);
}

void GXUseTexture(GXTexture* tex)
{
	glBindTexture(GL_TEXTURE_2D, tex->tex);
}
