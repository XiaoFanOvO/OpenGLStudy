#version 460 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D sampler; //diffuse贴图采样器
uniform sampler2D specularMaskSampler; //高光蒙版采样器

//光源参数
//uniform vec3 lightPosition;
//uniform vec3 targetDirection;//探照灯的指向
//uniform vec3 lightColor;
//可以将inner和outer两个角度的cos值在cpu计算完毕,传入shader
//uniform float innerAngle;//这里必须是弧度值
//uniform float outerAngle;//这里必须是弧度值
//uniform float innerLine; //cos theta
//uniform float outerLine; //cos β
//uniform float k2;
//uniform float k1;
//uniform float kc;
//uniform float specularIntensity;
uniform vec3 ambientColor;

//相机位置
uniform vec3 cameraPosition;

uniform float shiness;

//三种光源的封装
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
#define POINT_LIGHT_NUM 4 //宏定义
uniform PointLight pointLights[POINT_LIGHT_NUM];

//计算漫反射光照
vec3 calculateDiffuse(vec3 lightColor, vec3 objectColor, vec3 lightDir, vec3 normal){
	//2 准备diffuse（漫反射）相关的各类数据 
	float diffuse = clamp(dot(-lightDir, normal), 0.0,1.0);
	vec3 diffuseColor = lightColor * diffuse * objectColor;
	return diffuseColor;
}

//计算镜面反射的光照
vec3 calculateSpecular(vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir, float intensity){
	//1 防止背面光的照入
	float dotResult = dot(-lightDir, normal);
	float flag = step(0.0, dotResult);
	vec3 lightReflect = normalize(reflect(lightDir, normal));
	//2 计算specularIntensity
	float specular = clamp(dot(lightReflect, -viewDir),0.0,1.0);
	//3 计算光斑
	specular = pow(specular, shiness);
	//float sepcularMask = texture(specularMaskSampler, uv).r; //高光贴图,需要再点开
	//4 计算最终颜色
	vec3 specularColor = lightColor * specular * flag * intensity;
	return specularColor;
}

//计算聚光灯
vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir){
	//1 获取物体的当前像素的颜色
	vec3 objectColor  = texture(sampler, uv).xyz;
	vec3 lightDir = normalize(worldPosition - light.position);
	vec3 targetDir = normalize(light.targetDirection);

	//计算spotLight的照射范围
	//float cTheta = dot(lightDirN, targetDirN);
	//float cVisible = cos(visibleAngle);
	//float spotFlag = step(cVisible, cTheta);
	float cGamma = dot(lightDir, targetDir);
	float intensity = clamp((cGamma - light.outerLine) / (light.innerLine - light.outerLine),0.0,1.0);
	//1 准备diffuse（漫反射）相关的各类数据 
	vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
	//2 准备高光specular
	vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
	return (diffuseColor + specularColor) * intensity;
}

//计算平行光
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir){
	vec3 objectColor  = texture(sampler, uv).xyz;
	vec3 lightDir = normalize(light.direction);
	
	//1 准备diffuse（漫反射）相关的各类数据 
	vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
	//2 准备高光specular
	vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
	return diffuseColor + specularColor;
}

//计算点光源
vec3 calculatePointLight(PointLight light, vec3 normal, vec3 viewDir){
	vec3 objectColor  = texture(sampler, uv).xyz;
	vec3 lightDir = normalize(worldPosition - light.position);

	//计算衰减值
	float dist = length(worldPosition - light.position);//光源与像素的距离
	float attenuation = 1.0 / (light.k2 * dist * dist + light.k1 * dist + light.kc);//衰减值
	//1 准备diffuse（漫反射）相关的各类数据 
	vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
	//2 准备高光specular
	vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
	return (diffuseColor + specularColor) * attenuation;
}


void main()
{
	vec3 result = vec3(0.0,0.0,0.0);
	//计算光照的通用数据
	//1 获取物体的当前像素的颜色
	vec3 objectColor  = texture(sampler, uv).xyz;
	vec3 normalN = normalize(normal);
	//vec3 lightDirN = normalize(worldPosition - spotLight.position);
	vec3 viewDir = normalize(worldPosition - cameraPosition);
	//vec3 targetDirN = normalize(spotLight.targetDirection);

	//result += calculateSpotLight(spotLight, normalN, viewDir);
	result += calculateDirectionalLight(directionalLight, normalN, viewDir);
	//遍历点光源数组
	//for(int i = 0; i < POINT_LIGHT_NUM; i++){
	//	result += calculatePointLight(pointLights[i], normalN, viewDir);
	//}

	//环境光
	vec3 ambientColor = objectColor * ambientColor;
	vec3 finalColor = result  + ambientColor;
	FragColor = vec4(finalColor, 1.0);
}