#version 310 es

precision highp int;
precision highp float;
precision highp sampler2D;

#define	TYPE_IGNORE	0u
#define	TYPE_OUTLINE	1u
#define	TYPE_FILL	2u
#define	TYPE_GLYPH	3u

uniform sampler2D font;
uniform uint keys_down[4];

in       vec2 texcoord;
in  flat uint key_id;
in  flat uint vtx_type;
out      vec4 color;

const vec4 color_outline_inactive = vec4(0.00, 0.00, 1.00, 1.0);
const vec4 color_outline_active   = vec4(0.00, 1.00, 1.00, 1.0);
const vec4 color_fill_inactive    = vec4(0.00, 0.06, 0.12, 1.0);
const vec4 color_fill_active      = vec4(0.00, 0.25, 0.50, 1.0);
const vec4 color_glyph_inactive   = vec4(0.00, 0.50, 1.00, 1.0);
const vec4 color_glyph_active     = vec4(0.00, 1.00, 1.00, 1.0);

vec4 get_glyph(bool act)
{
	vec4 texel = texture(font, texcoord);
	float range = 0.050;
	float alpha = smoothstep(0.5 - range, 0.5 + range, texel.r);
	vec4 col = act ? color_glyph_active : color_glyph_inactive;
	return vec4(col.rgb, alpha);
}

void main(void)
{
	uint idx = key_id / 32u;
	uint bit = key_id % 32u;
	bool act = (keys_down[idx] & (1u << bit)) != 0u;
	switch(vtx_type) {
		case TYPE_OUTLINE:
			color = act ? color_outline_active
				: color_outline_inactive;
			break;
		case TYPE_FILL:
			color = act ? color_fill_active
				: color_fill_inactive;
			break;
		case TYPE_GLYPH:
			color = get_glyph(act);
			break;
		case TYPE_IGNORE:
			discard;
		default:
			color = vec4(1.0, 0.0, 0.0, 1.0);
			break;
	}
}
