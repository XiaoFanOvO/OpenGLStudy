#version 460 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;


void main()
{
	FragColor = vec4(vec3(1.0,1.0,1.0), 1.0);
}