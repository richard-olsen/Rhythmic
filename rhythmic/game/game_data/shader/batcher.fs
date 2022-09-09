#version 150

uniform sampler2D textureSample;

uniform float fogPos;
//fogPos = 1.6
//fogSize = 0.2

in vec2 tex_coord;
in vec4 color_val;
in vec4 pos;

out vec4 fragColor;

void main()
{
	if ((pos.y / pos.w) < -1)
		discard;
	float z = (gl_FragCoord.z * gl_FragCoord.w) * 13;
	float fogVal = clamp((fogPos - z) / -0.2, 0, 1);
	fragColor = (texture(textureSample, tex_coord) * color_val * fogVal);
}
