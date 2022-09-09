#version 150

in vec3 position;
in vec2 texture;
in vec4 color;

out vec2 tex_coord;
out vec4 color_val;
out vec4 pos;

uniform mat4 modelview;
uniform mat4 cameraTransform;
uniform mat4 projection;

uniform mat4 screenPos;

void main()
{
	color_val = color;
	tex_coord = texture;
	pos = (projection * cameraTransform * modelview) * vec4(position, 1);
	gl_Position = screenPos * pos;
}
