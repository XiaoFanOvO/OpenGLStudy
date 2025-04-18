#version 460 core
layout (location = 0) in vec3 aPos;


out vec3 uvw;
out vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


void main()
{
	vec4 transformPosition = vec4(aPos, 1.0);

	transformPosition = modelMatrix * transformPosition;


	gl_Position = projectionMatrix * viewMatrix * transformPosition;

	gl_Position = gl_Position.xyww;

	uvw = aPos;

} 