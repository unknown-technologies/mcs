#include <mcs/types.h>
#include <mcs/kb.h>

/**********************************************************************/
static const KBKey layout_qwerty_layer0_cols_0[] = {
	{ .id =  1, .code = '`' },
	{ .id =  2, .code = '1' },
	{ .id =  3, .code = '2' },
	{ .id =  4, .code = '3' },
	{ .id =  5, .code = '4' },
	{ .id =  6, .code = '5' },
	{ .id =  7, .code = '6' },
	{ .id =  8, .code = '7' },
	{ .id =  9, .code = '8' },
	{ .id = 10, .code = '9' },
	{ .id = 11, .code = '0' },
	{ .id = 12, .code = '-' },
	{ .id = 13, .code = '=' }
};

static const KBKey layout_qwerty_layer0_cols_1[] = {
	{ .id = 14, .code = VK_SPACE },
	{ .id = 15, .code = 'q' },
	{ .id = 16, .code = 'w' },
	{ .id = 17, .code = 'e' },
	{ .id = 18, .code = 'r' },
	{ .id = 19, .code = 't' },
	{ .id = 20, .code = 'y' },
	{ .id = 21, .code = 'u' },
	{ .id = 22, .code = 'i' },
	{ .id = 23, .code = 'o' },
	{ .id = 24, .code = 'p' },
	{ .id = 25, .code = '[' },
	{ .id = 26, .code = ']' }
};

static const KBKey layout_qwerty_layer0_cols_2[] = {
	{ .id = 27, .code = VK_SHIFT },
	{ .id = 28, .code = 'a' },
	{ .id = 29, .code = 's' },
	{ .id = 30, .code = 'd' },
	{ .id = 31, .code = 'f' },
	{ .id = 32, .code = 'g' },
	{ .id = 33, .code = 'h' },
	{ .id = 34, .code = 'j' },
	{ .id = 35, .code = 'k' },
	{ .id = 36, .code = 'l' },
	{ .id = 37, .code = ';' },
	{ .id = 38, .code = '\'' },
	{ .id = 39, .code = '\\' }
};

static const KBKey layout_qwerty_layer0_cols_3[] = {
	{ .id = 40, .code = '<' },
	{ .id = 41, .code = 'z' },
	{ .id = 42, .code = 'x' },
	{ .id = 43, .code = 'c' },
	{ .id = 44, .code = 'v' },
	{ .id = 45, .code = 'b' },
	{ .id = 46, .code = 'n' },
	{ .id = 47, .code = 'm' },
	{ .id = 48, .code = ',' },
	{ .id = 49, .code = '.' },
	{ .id = 50, .code = '/' },
	{ .id = 51, .code = VK_DELETE },
	{ .id = 52, .code = VK_ENTER }
};

static const KBRow layout_qwerty_rows_layer0[] = {
	{ .count = 13, .keys = layout_qwerty_layer0_cols_0 },
	{ .count = 13, .keys = layout_qwerty_layer0_cols_1 },
	{ .count = 13, .keys = layout_qwerty_layer0_cols_2 },
	{ .count = 13, .keys = layout_qwerty_layer0_cols_3 }
};

/**********************************************************************/
static const KBKey layout_qwerty_layer1_cols_0[] = {
	{ .id =  1, .code = '~' },
	{ .id =  2, .code = '!' },
	{ .id =  3, .code = '@' },
	{ .id =  4, .code = '#' },
	{ .id =  5, .code = '$' },
	{ .id =  6, .code = '%' },
	{ .id =  7, .code = '^' },
	{ .id =  8, .code = '&' },
	{ .id =  9, .code = '*' },
	{ .id = 10, .code = '(' },
	{ .id = 11, .code = ')' },
	{ .id = 12, .code = '_' },
	{ .id = 13, .code = '+' }
};

static const KBKey layout_qwerty_layer1_cols_1[] = {
	{ .id = 14, .code = VK_SPACE },
	{ .id = 15, .code = 'Q' },
	{ .id = 16, .code = 'W' },
	{ .id = 17, .code = 'E' },
	{ .id = 18, .code = 'R' },
	{ .id = 19, .code = 'T' },
	{ .id = 20, .code = 'Y' },
	{ .id = 21, .code = 'U' },
	{ .id = 22, .code = 'I' },
	{ .id = 23, .code = 'O' },
	{ .id = 24, .code = 'P' },
	{ .id = 25, .code = '{' },
	{ .id = 26, .code = '}' }
};

static const KBKey layout_qwerty_layer1_cols_2[] = {
	{ .id = 27, .code = VK_SHIFT },
	{ .id = 28, .code = 'A' },
	{ .id = 29, .code = 'S' },
	{ .id = 30, .code = 'D' },
	{ .id = 31, .code = 'F' },
	{ .id = 32, .code = 'G' },
	{ .id = 33, .code = 'H' },
	{ .id = 34, .code = 'J' },
	{ .id = 35, .code = 'K' },
	{ .id = 36, .code = 'L' },
	{ .id = 37, .code = ':' },
	{ .id = 38, .code = '"' },
	{ .id = 39, .code = '|' }
};

static const KBKey layout_qwerty_layer1_cols_3[] = {
	{ .id = 40, .code = '>' },
	{ .id = 41, .code = 'Z' },
	{ .id = 42, .code = 'X' },
	{ .id = 43, .code = 'C' },
	{ .id = 44, .code = 'V' },
	{ .id = 45, .code = 'B' },
	{ .id = 46, .code = 'N' },
	{ .id = 47, .code = 'M' },
	{ .id = 48, .code = ',' },
	{ .id = 49, .code = '.' },
	{ .id = 50, .code = '?' },
	{ .id = 51, .code = VK_DELETE },
	{ .id = 52, .code = VK_ENTER }
};

static const KBRow layout_qwerty_rows_layer1[] = {
	{ .count = 13, .keys = layout_qwerty_layer1_cols_0 },
	{ .count = 13, .keys = layout_qwerty_layer1_cols_1 },
	{ .count = 13, .keys = layout_qwerty_layer1_cols_2 },
	{ .count = 13, .keys = layout_qwerty_layer1_cols_3 }
};

static const KBLayer layout_qwerty_layers[] = {
	{ .modifiers = {  0, 0 }, .rows = layout_qwerty_rows_layer0 },
	{ .modifiers = { 27, 0 }, .rows = layout_qwerty_rows_layer1 }
};

const KBLayout keyboard_layout_qwerty = {
	.row_cnt = 4,
	.col_cnt = 13,
	.layer_cnt = 2,
	.layers = layout_qwerty_layers
};
