#version 330

layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
uniform mat4 mvMat;
uniform mat4 projectionMat;

void main()
{
	vec4 v = vec4(position, 1);
	mat4 mvp = projectionMat * mvMat;
	gl_Position = mvp * v;
}
