#version 460 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;
in vec4 lightSpaceClipCoord;

uniform sampler2D sampler;	//diffuse贴图采样器
uniform sampler2D specularMaskSampler;//specularMask贴图采样器

uniform sampler2D shadowMapSampler;
uniform float bias;

uniform vec3 ambientColor;

//相机世界位置
uniform vec3 cameraPosition;


uniform float shiness;

//透明度
uniform float opacity;


struct DirectionalLight{
	vec3 direction;
	vec3 color;
	float specularIntensity;
	float intensity;
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

uniform DirectionalLight directionalLight;

//计算漫反射光照
vec3 calculateDiffuse(vec3 lightColor, vec3 objectColor, vec3 lightDir, vec3 normal){
	float diffuse = clamp(dot(-lightDir, normal), 0.0,1.0);
	vec3 diffuseColor = lightColor * diffuse * objectColor;

	return diffuseColor;
}

//计算镜面反射光照
vec3 calculateSpecular(vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir, float intensity){
	//1 防止背面光效果
	float dotResult = dot(-lightDir, normal);
	float flag = step(0.0, dotResult);
	vec3 lightReflect = normalize(reflect(lightDir,normal));

	//2 jisuan specular
	float specular = max(dot(lightReflect,-viewDir), 0.0);

	//3 控制光斑大小
	specular = pow(specular, shiness);

	//float specularMask = texture(specularMaskSampler, uv).r;

	//4 计算最终颜色
	vec3 specularColor = lightColor * specular * flag * intensity;

	return specularColor;
}

vec3 calculateSpotLight(vec3 objectColor, SpotLight light, vec3 normal, vec3 viewDir){
	//计算光照的通用数据
	vec3 lightDir = normalize(worldPosition - light.position);
	vec3 targetDir = normalize(light.targetDirection);

	//计算spotlight的照射范围
	float cGamma = dot(lightDir, targetDir);
	float intensity =clamp( (cGamma - light.outerLine) / (light.innerLine - light.outerLine), 0.0, 1.0);

	//1 计算diffuse
	vec3 diffuseColor = calculateDiffuse(light.color,objectColor, lightDir,normal);

	//2 计算specular
	vec3 specularColor = calculateSpecular(light.color, lightDir,normal, viewDir,light.specularIntensity); 

	return (diffuseColor + specularColor)*intensity;
}

vec3 calculateDirectionalLight(vec3 objectColor, DirectionalLight light, vec3 normal ,vec3 viewDir){
	light.color *= light.intensity;

	//计算光照的通用数据
	vec3 lightDir = normalize(light.direction);

	//1 计算diffuse
	vec3 diffuseColor = calculateDiffuse(light.color,objectColor, lightDir,normal);

	//2 计算specular
	vec3 specularColor = calculateSpecular(light.color, lightDir,normal, viewDir,light.specularIntensity); 

	return diffuseColor + specularColor;
}

vec3 calculatePointLight(vec3 objectColor, PointLight light, vec3 normal ,vec3 viewDir){
	//计算光照的通用数据
	vec3 lightDir = normalize(worldPosition - light.position);

	//计算衰减
	float dist = length(worldPosition - light.position);
	float attenuation = 1.0 / (light.k2 * dist * dist + light.k1 * dist + light.kc);

	//1 计算diffuse
	vec3 diffuseColor = calculateDiffuse(light.color,objectColor, lightDir,normal);

	//2 计算specular
	vec3 specularColor = calculateSpecular(light.color, lightDir,normal, viewDir,light.specularIntensity); 

	return (diffuseColor + specularColor)*attenuation;
}

float calculateShadow(){
	//1 找到当前像素在光源空间内的NDC坐标
	vec3 lightNDC = lightSpaceClipCoord.xyz / lightSpaceClipCoord.w;
	//2 找到当前像素在ShadowMap上的uv
	vec3 projCoord = lightNDC * 0.5 + 0.5;//这里0.5会被自动扩展为三维数组
	vec2 uv = projCoord.xy;
	//3 使用这个uv对shaderMap进行采样,得到ClosestDepth
	float closestDepth = texture(shadowMapSampler, uv).r;//深度贴图只有一个通道,也就是r
	//4 对比当前像素在光源空间内的深度值与ClosestDepth的大小
	float selfDepth = projCoord.z;

	float shadow = (selfDepth - bias) > closestDepth ? 1.0 : 0.0;
	return shadow;
}

void main()
{
//环境光计算
	vec3 objectColor  = texture(sampler, uv).xyz ;
	vec3 result = vec3(0.0,0.0,0.0);

	//计算光照的通用数据
	vec3 normalN = normalize(normal);
	vec3 viewDir = normalize(worldPosition - cameraPosition);

	result += calculateDirectionalLight(objectColor, directionalLight,normalN, viewDir);

	
	float alpha =  texture(sampler, uv).a;

	vec3 ambientColor = objectColor * ambientColor;
	
	float shadow = calculateShadow();
	vec3 finalColor = result * (1.0 - shadow) + ambientColor;
	

	FragColor = vec4(finalColor,alpha * opacity);
}