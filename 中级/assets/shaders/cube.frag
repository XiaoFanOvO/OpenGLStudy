#version 460 core
out vec4 FragColor;

in vec3 uvw;
uniform sampler2D sphericalSampler;

const float PI = 3.1415926535897;

vec2 uvwToUv(vec3 uvwN){
	float phi = asin(uvwN.y);
	float theta = atan(uvwN.z, uvwN.x);
	float u = theta / (2.0 * PI) + 0.5;
	float v = phi / PI + 0.5;
	return vec2(u,v);
}

void main()
{
	vec3 uvwN = normalize(uvw);
	vec2 uv = uvwToUv(uvwN);
	FragColor = texture(sphericalSampler, uv);
}