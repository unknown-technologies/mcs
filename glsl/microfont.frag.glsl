#version 310 es

precision highp float;
precision highp int;
precision highp usampler2D;

uniform vec4 textcolor;
uniform uint glyph;
uniform usampler2D font;

in  vec2 pos;
out vec4 color;

void main(void)
{
	ivec2 texcoords = ivec2(pos.y * 9.0, int(glyph));
	uint bits = texelFetch(font, texcoords, 0).r;
	int px = int(pos * 5.0);
	bool bit = ((bits << px) & 16u) != 0u;

	if(bit) {
		color = textcolor;
	} else {
		color = vec4(0.0);
	}
}
