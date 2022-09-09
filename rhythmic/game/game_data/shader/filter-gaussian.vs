#version 150

in vec3 position;
in vec2 texture;
in vec4 color;

out vec2 tex_coord;

void main()
{
	tex_coord = texture;
	gl_Position = vec4(position, 1);
}
