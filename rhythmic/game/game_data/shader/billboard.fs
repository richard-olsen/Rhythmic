#version 150

uniform sampler2D textureSample;
uniform float fogPos;

in vec2 tex_coord;
in vec4 color_val;
in vec4 pos;

out vec4 fragColor;

void main()
{
	//if ((pos.y / pos.w) < -1)
	//	discard;

	vec4 outcolor = texture(textureSample, tex_coord);
	if (outcolor.a <= 0)
		discard;
	
	float z = (gl_FragCoord.z * gl_FragCoord.w) * 13;
	float fogVal = clamp((fogPos - z) / -0.2, 0, 1);
	
	if (fogVal <= 0)
		discard;
	fragColor = outcolor * color_val * fogVal;// + vec4(0,0,0,0.5);
}
