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

uniform vec3 point_center;

void main()
{
	color_val = color;
	tex_coord = texture;

	mat4 transform = cameraTransform * modelview;

	vec3 camera_right = vec3(transform[0][0], transform[1][0], transform[2][0]);
	vec3 camera_up = vec3(transform[0][1], transform[1][1], transform[2][1]);

	pos = (projection * transform) *
		vec4(
			point_center +
			(camera_right * position.x) +
			(camera_up * position.y),
			1
		);

	gl_Position = screenPos * pos;
}
