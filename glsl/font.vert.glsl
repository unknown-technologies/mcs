#version 310 es

precision highp float;
precision highp int;
precision highp sampler2D;
precision highp usampler2D;

layout(location = 0) in vec3 position;

uniform float scale;
uniform vec2 screen_size;
uniform vec2 textpos;
uniform uint glyph;
uniform float maxbaseline;
uniform sampler2D metrics;
uniform usampler2D charmap;

out vec2 pos;

int s16(uint x)
{
	if(x > 0x7FFFu) {
		return 0x10000 - int(x);
	} else {
		return int(x);
	}
}

void main(void)
{
	uint charid = texelFetch(charmap, ivec2(int(glyph), 0), 0).r;
	vec4 metric = texelFetch(metrics, ivec2(0, int(charid)), 0);
	vec4 extra = texelFetch(metrics, ivec2(1, int(charid)), 0);
	vec2 glyph_size = vec2(metric.zw);
	float startX = extra.x;
	float baseline = extra.y;
	float startY = maxbaseline - baseline;
	vec2 off = vec2(-startX, startY) * scale;

	vec2 glyphpos = (position.xy + vec2(1.0, 1.0)) / 2.0;
	vec2 pos2D = textpos + off + glyphpos * glyph_size * scale;
	vec2 pos3D = pos2D / screen_size * 2.0 - vec2(1.0, 1.0);

	gl_Position = vec4(pos3D.x, -pos3D.y, position.z, 1.0);

	pos = (position.xy + vec2(1.0, 1.0)) / 2.0;
}
