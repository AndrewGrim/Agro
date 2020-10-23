#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec4 aRect;

out vec4 vColor;

uniform mat4 projection;

void main()
{
	mat4 model = mat4(
		vec4(aRect.z, 0.0, 0.0, 0.0),
		vec4(0.0, aRect.w, 0.0, 0.0),
		vec4(0.0, 0.0, 1.0, 0.0),
		vec4(aRect.x, aRect.y, 0.0, 1.0)
	);
	gl_Position = projection * model * vec4(aPos, 1.0, 1.0);
	vColor = aColor;
}