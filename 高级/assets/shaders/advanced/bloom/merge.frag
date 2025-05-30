#version 460 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D originTex;
uniform sampler2D bloomTex;

uniform float bloomIntensity;
void main()
{
	vec3 originColor = texture(originTex, uv).rgb;
	vec3 bloomColor = texture(bloomTex, uv).rgb;
	vec3 color = originColor + bloomColor * bloomIntensity;

	FragColor = vec4(color, 1.0f);
}