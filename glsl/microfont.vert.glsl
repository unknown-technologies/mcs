#version 310 es

precision highp float;
precision highp int;
precision highp usampler2D;

layout(location = 0) in vec3 position;

uniform vec2 screen_size;
uniform vec2 textpos;

out vec2 pos;

void main(void)
{
	vec2 glyphpos = (position.xy + vec2(1.0, 1.0)) / 2.0;
	vec2 pos2D = textpos + glyphpos * vec2(5.0, 9.0);
	vec2 pos3D = pos2D / screen_size * 2.0 - vec2(1.0, 1.0);

	gl_Position = vec4(pos3D.x, -pos3D.y, position.z, 1.0);

	pos = (position.xy + vec2(1.0, 1.0)) / 2.0;
}
