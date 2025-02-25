#ifndef __KB_H__
#define __KB_H__

#include <mcs/types.h>
#include <mcs/shader.h>
#include <mcs/font.h>
#include <mcs/mt.h>

#define	VK_0			'0'
#define	VK_1			'1'
#define	VK_2			'2'
#define	VK_3			'3'
#define	VK_4			'4'
#define	VK_5			'5'
#define	VK_6			'6'
#define	VK_7			'7'
#define	VK_8			'8'
#define	VK_9			'9'
#define	VK_A			'A'
#define	VK_B			'B'
#define	VK_C			'C'
#define	VK_D			'D'
#define	VK_E			'E'
#define	VK_F			'F'
#define	VK_G			'G'
#define	VK_H			'H'
#define	VK_I			'I'
#define	VK_J			'J'
#define	VK_K			'K'
#define	VK_L			'L'
#define	VK_M			'M'
#define	VK_N			'N'
#define	VK_O			'O'
#define	VK_P			'P'
#define	VK_Q			'Q'
#define	VK_R			'R'
#define	VK_S			'S'
#define	VK_T			'T'
#define	VK_U			'U'
#define	VK_V			'V'
#define	VK_W			'W'
#define	VK_X			'X'
#define	VK_Y			'Y'
#define	VK_Z			'Z'
#define	VK_MINUS		'-'
#define	VK_EQUAL		'='
#define	VK_LEFT_BRACKET		'['
#define	VK_RIGHT_BRACKET	']'
#define	VK_COLON		':'
#define	VK_SEMICOLON		';'
#define	VK_SLASH		'/'
#define	VK_BACKSLASH		'\\'
#define	VK_BACKTICK		'`'
#define	VK_QUOTE		'"'
#define	VK_APOSTROPHE		'\''
#define	VK_COMMA		','
#define	VK_PERIOD		'.'
#define	VK_LESS_THAN		'<'
#define	VK_GREATER_THAN		'>'
#define	VK_SPACE		' '
#define	VK_ENTER		'\r'
#define	VK_DELETE		0x7F
#define	VK_ESC			0x1B
#define	VK_F1			0x201
#define	VK_F2			0x202
#define	VK_F3			0x203
#define	VK_F4			0x204
#define	VK_F5			0x205
#define	VK_F6			0x206
#define	VK_F7			0x207
#define	VK_F8			0x208
#define	VK_F9			0x209
#define	VK_F10			0x20A
#define	VK_F11			0x20B
#define	VK_F12			0x20C
#define	VK_SHIFT		0x300
#define	VK_CTRL			0x301

#define	KB_POSITION_TOP		TRUE
#define	KB_POSITION_BOTTOM	FALSE

typedef struct {
	unsigned int	id;
	unsigned int	code;
} KBKey;

typedef struct {
	unsigned int	count;
	const KBKey*	keys;
} KBRow;

typedef struct {
	unsigned int	modifiers[2];
	const KBRow*	rows;
} KBLayer;

typedef struct {
	unsigned int	row_cnt;
	unsigned int	col_cnt;
	unsigned int	layer_cnt;
	const KBLayer*	layers;
} KBLayout;

typedef struct {
	float		x;
	float		y;
	float		u;
	float		v;
	GLuint		id;
	GLuint		type;
} KBVTX;

typedef struct {
	void		(*callback)(unsigned int code, void* userdata);
	void*		userdata;
	GXTexture*	font;
	KBVTX*		vertices;
	const KBLayout*	layout;
	GLuint*		vao;
	GLuint*		vbo;
	unsigned int	vtx_cnt;
	unsigned int	current_layer;
	float		height;
	GXShader	shader;
	GLuint		shader_font;
	GLuint		shader_offset_y;
	GLuint		shader_screensz;
	GLuint		shader_keys_down;
	float		cell_size;
	unsigned int	keys_down[4];
	unsigned int	last_down[4];
	BOOL		top;
} KB;

extern const KBLayout keyboard_layout_qwerty;

void	KBInit(KB* kb, GXFont* font, const KBLayout* layout, int width,
		int height);
void	KBDestroy(KB* kb);

void	KBProcess(KB* kb);
void	KBProcessInput(KB* kb, MT_SLOT* slot);
void	KBDraw(KB* kb);

static inline void KBSetPosition(KB* kb, BOOL top)
{
	kb->top = top;
}

static inline float KBGetHeight(KB* kb)
{
	return kb->height;
}

static inline void KBSetCallback(KB* kb,
		void (*callback)(unsigned int, void*))
{
	kb->callback = callback;
}

static inline void KBSetUserData(KB* kb, void* userdata)
{
	kb->userdata = userdata;
}

static inline void* KBGetUserData(KB* kb)
{
	return kb->userdata;
}

#endif
