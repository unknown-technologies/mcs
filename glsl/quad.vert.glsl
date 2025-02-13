#version 310 es

precision highp float;

layout(location = 0) in vec3 position;

uniform vec2 screen_size;
uniform vec4 quad;

out vec2 pos;

void main(void)
{
	vec2 quadpos = (position.xy + vec2(1.0, 1.0)) / 2.0;
	vec2 pos2D = quad.xy + quadpos * quad.zw;
	vec2 pos3D = pos2D / screen_size * 2.0 - vec2(1.0, 1.0);

	gl_Position = vec4(pos3D.x, -pos3D.y, position.z, 1.0);

	pos = (position.xy + vec2(1.0, 1.0)) / 2.0;
}
