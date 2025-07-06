#version 310 es

precision highp float;
precision highp sampler2D;

uniform vec4 quadcolor;

in  vec2 pos;
out vec4 color;

void main(void)
{
	color = quadcolor;
}
