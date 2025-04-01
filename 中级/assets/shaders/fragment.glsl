#version 460 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D sampler;

//光源参数
uniform vec3 lightDirection;
uniform vec3 lightColor;

//相机位置
uniform vec3 cameraPosition;

void main()
{
	//计算光照的通用数据
	//1 获取物体的当前像素的颜色
	vec3 objectColor  = texture(sampler, uv).xyz;
	vec3 normalN = normalize(normal);
	vec3 lightDirN = normalize(lightDirection);
	vec3 viewDir = normalize(worldPosition - cameraPosition);
	//2 准备diffuse（漫反射）相关的各类数据 
	float diffuse = clamp(dot(-lightDirN, normalN), 0.0,1.0);
	vec3 diffuseColor = lightColor * diffuse * objectColor;
	//3 准备高光specular
	vec3 lightReflect = normalize(reflect(lightDirN, normalN));
	float specular = clamp(dot(lightReflect, -viewDir),0.0,1.0);
	vec3 specularColor = lightColor * specular;

	vec3 finalColor = diffuseColor + specularColor;

	FragColor = vec4(finalColor, 1.0);
}