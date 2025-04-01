#version 460 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D sampler;

//��Դ����
uniform vec3 lightDirection;
uniform vec3 lightColor;

//���λ��
uniform vec3 cameraPosition;

void main()
{
	//������յ�ͨ������
	//1 ��ȡ����ĵ�ǰ���ص���ɫ
	vec3 objectColor  = texture(sampler, uv).xyz;
	vec3 normalN = normalize(normal);
	vec3 lightDirN = normalize(lightDirection);
	vec3 viewDir = normalize(worldPosition - cameraPosition);
	//2 ׼��diffuse�������䣩��صĸ������� 
	float diffuse = clamp(dot(-lightDirN, normalN), 0.0,1.0);
	vec3 diffuseColor = lightColor * diffuse * objectColor;
	//3 ׼���߹�specular
	vec3 lightReflect = normalize(reflect(lightDirN, normalN));
	float specular = clamp(dot(lightReflect, -viewDir),0.0,1.0);
	vec3 specularColor = lightColor * specular;

	vec3 finalColor = diffuseColor + specularColor;

	FragColor = vec4(finalColor, 1.0);
}