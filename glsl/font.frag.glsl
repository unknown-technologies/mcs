#version 310 es

precision highp float;
precision highp int;
precision highp sampler2D;
precision highp usampler2D;

uniform vec4 textcolor;
uniform uint glyph;
uniform sampler2D font;
uniform sampler2D metrics;
uniform usampler2D charmap;

in  vec2 pos;
out vec4 color;

void main(void)
{
	uint charid = texelFetch(charmap, ivec2(int(glyph), 0), 0).r;
	vec4 metric = texelFetch(metrics, ivec2(0, int(charid)), 0);

	ivec2 font_size = textureSize(font, 0);
	vec2 glyph_pos = vec2(metric.xy);
	vec2 glyph_size = vec2(metric.zw);
	vec2 texcoords = glyph_pos + (pos * glyph_size);
	vec4 texcolor = texture(font, texcoords / vec2(font_size));

	float range = 0.025;
	float alpha = smoothstep(0.5 - range, 0.5 + range, texcolor.r);
	color = vec4(textcolor.rgb, alpha);
}
