#version 310 es

precision highp float;

uniform vec2 screen_size;
uniform float offsetY;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in uint id;
layout(location = 3) in uint type;

out      vec2 texcoord;
out flat uint key_id;
out flat uint vtx_type;

void main(void)
{
	texcoord = uv;
	key_id = id;
	vtx_type = type;

	vec2 pos = (position + vec2(0.0, offsetY)) / screen_size;
	pos = (pos * vec2(2.0)) - vec2(1.0);
	gl_Position = vec4(pos.x, -pos.y, 0.0, 1.0);
}
