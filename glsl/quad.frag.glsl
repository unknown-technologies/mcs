#version 310 es

precision highp float;
precision highp sampler2D;

uniform bool sdf;
uniform vec4 quadcolor;
uniform sampler2D tex;

const float range = 0.025;

in  vec2 pos;
out vec4 color;

void main(void)
{
	vec4 texcolor = texture(tex, pos);

	if(sdf) {
		float alpha = smoothstep(0.5 - range, 0.5 + range,
				texcolor.r);
		color = vec4(quadcolor.rgb, alpha);
	} else {
		color = texcolor;
	}
}
