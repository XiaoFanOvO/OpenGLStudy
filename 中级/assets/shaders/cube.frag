#version 460 core
out vec4 FragColor;

in vec2 uv;
uniform sampler2D diffuse;

void main()
{
	
 
	FragColor = vec4(texture(diffuse, uv).rgb, 1.0);
}