#version 460 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D sampler; //diffuse��ͼ������
uniform sampler2D specularMaskSampler; //�߹��ɰ������

//��Դ����
//uniform vec3 lightPosition;
//uniform vec3 targetDirection;//̽�յƵ�ָ��
//uniform vec3 lightColor;
//���Խ�inner��outer�����Ƕȵ�cosֵ��cpu�������,����shader
//uniform float innerAngle;//��������ǻ���ֵ
//uniform float outerAngle;//��������ǻ���ֵ
//uniform float innerLine; //cos theta
//uniform float outerLine; //cos ��
//uniform float k2;
//uniform float k1;
//uniform float kc;
//uniform float specularIntensity;
uniform vec3 ambientColor;

//���λ��
uniform vec3 cameraPosition;

uniform float shiness;

//���ֹ�Դ�ķ�װ
struct DirectionalLight{
	vec3 direction;
	vec3 color;
	float specularIntensity;
};

struct PointLight{
	vec3 position;
	vec3 color;
	float specularIntensity;

	float k2;
	float k1;
	float kc;
};

struct SpotLight{
	vec3 position;
	vec3 targetDirection;
	vec3 color;
	float outerLine;
	float innerLine;
	float specularIntensity;
};

uniform SpotLight spotLight;
uniform DirectionalLight directionalLight;
#define POINT_LIGHT_NUM 4 //�궨��
uniform PointLight pointLights[POINT_LIGHT_NUM];

//�������������
vec3 calculateDiffuse(vec3 lightColor, vec3 objectColor, vec3 lightDir, vec3 normal){
	//2 ׼��diffuse�������䣩��صĸ������� 
	float diffuse = clamp(dot(-lightDir, normal), 0.0,1.0);
	vec3 diffuseColor = lightColor * diffuse * objectColor;
	return diffuseColor;
}

//���㾵�淴��Ĺ���
vec3 calculateSpecular(vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir, float intensity){
	//1 ��ֹ����������
	float dotResult = dot(-lightDir, normal);
	float flag = step(0.0, dotResult);
	vec3 lightReflect = normalize(reflect(lightDir, normal));
	//2 ����specularIntensity
	float specular = clamp(dot(lightReflect, -viewDir),0.0,1.0);
	//3 ������
	specular = pow(specular, shiness);
	//float sepcularMask = texture(specularMaskSampler, uv).r; //�߹���ͼ,��Ҫ�ٵ㿪
	//4 ����������ɫ
	vec3 specularColor = lightColor * specular * flag * intensity;
	return specularColor;
}

//����۹��
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir){
	//1 ��ȡ����ĵ�ǰ���ص���ɫ
	vec3 objectColor  = texture(sampler, uv).xyz;
	vec3 lightDir = normalize(worldPosition - light.position);
	vec3 targetDir = normalize(light.targetDirection);

	//����spotLight�����䷶Χ
	//float cTheta = dot(lightDirN, targetDirN);
	//float cVisible = cos(visibleAngle);
	//float spotFlag = step(cVisible, cTheta);
	float cGamma = dot(lightDir, targetDir);
	float intensity = clamp((cGamma - light.outerLine) / (light.innerLine - light.outerLine),0.0,1.0);
	//1 ׼��diffuse�������䣩��صĸ������� 
	vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
	//2 ׼���߹�specular
	vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
	return (diffuseColor + specularColor) * intensity;
}

//����ƽ�й�
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir){
	vec3 objectColor  = texture(sampler, uv).xyz;
	vec3 lightDir = normalize(light.direction);
	
	//1 ׼��diffuse�������䣩��صĸ������� 
	vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
	//2 ׼���߹�specular
	vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
	return diffuseColor + specularColor;
}

//������Դ
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir){
	vec3 objectColor  = texture(sampler, uv).xyz;
	vec3 lightDir = normalize(worldPosition - light.position);

	//����˥��ֵ
	float dist = length(worldPosition - light.position);//��Դ�����صľ���
	float attenuation = 1.0 / (light.k2 * dist * dist + light.k1 * dist + light.kc);//˥��ֵ
	//1 ׼��diffuse�������䣩��صĸ������� 
	vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
	//2 ׼���߹�specular
	vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
	return (diffuseColor + specularColor) * attenuation;
}


void main()
{
	vec3 result = vec3(0.0,0.0,0.0);
	//������յ�ͨ������
	//1 ��ȡ����ĵ�ǰ���ص���ɫ
	vec3 objectColor  = texture(sampler, uv).xyz;
	vec3 normalN = normalize(normal);
	//vec3 lightDirN = normalize(worldPosition - spotLight.position);
	vec3 viewDir = normalize(worldPosition - cameraPosition);
	//vec3 targetDirN = normalize(spotLight.targetDirection);

	//result += calculateSpotLight(spotLight, normalN, viewDir);
	result += calculateDirectionalLight(directionalLight, normalN, viewDir);
	//�������Դ����
	//for(int i = 0; i < POINT_LIGHT_NUM; i++){
	//	result += calculatePointLight(pointLights[i], normalN, viewDir);
	//}

	//������
	vec3 ambientColor = objectColor * ambientColor;
	vec3 finalColor = result  + ambientColor;
	FragColor = vec4(finalColor, 1.0);
}