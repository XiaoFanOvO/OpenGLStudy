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


#include "../common/commonLight.glsl"

uniform DirectionalLight directionalLight;

#define NUM_SAMPLES 32 
#define PI 3.141592653589793
#define PI2 6.283185307179586

float rand_2to1(vec2 uv ) { 
  // 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

uniform float diskTightness;
vec2 disk[NUM_SAMPLES];
//生成泊松采样盘
void poissonDiskSamples(vec2 randomSeed){
	//1 初始弧度
	float angle = rand_2to1(randomSeed) * PI2;

	//2 初始半径
	float radius = 1.0 / float(NUM_SAMPLES);

	//3 弧度步长(黄金分割)
	float angleStep = 3.883222077450933;

	//4 半径步长
	float radiusStep = radius;

	//5 循环生成
	for(int i = 0;i < NUM_SAMPLES;i++){
		disk[i] = vec2(cos(angle), sin(angle)) * pow(radius, diskTightness);
		radius += radiusStep;
		angle += angleStep;
	}
}



float getBias(vec3 normal, vec3 lightDir){
	vec3 normalN = normalize(normal);
	vec3 lightDirN = normalize(lightDir);
	
	return max(bias * (1.0 - dot(normalN, lightDirN)), 0.0005);
}

float calculateShadow(vec3 normal, vec3 lightDir){
	//1 找到当前像素在光源空间内的NDC坐标
	vec3 lightNDC = lightSpaceClipCoord.xyz/lightSpaceClipCoord.w;

	//2 找到当前像素在ShadowMap上的uv
	vec3 projCoord = lightNDC * 0.5 + 0.5;
	vec2 uv = projCoord.xy;

	//3 使用这个uv对ShadowMap进行采样，得到ClosestDepth
	float closestDepth = texture(shadowMapSampler, uv).r;

	//4 对比当前像素在光源空间内的深度值与ClosestDepth的大小
	float selfDepth = projCoord.z;

	float shadow = (selfDepth - getBias(normal, lightDir)) > closestDepth? 1.0:0.0;

	return shadow;
}

//float pcf(vec3 normal, vec3 lightDir){
//	//1 找到当前像素在光源空间内的NDC坐标
//	vec3 lightNDC = lightSpaceClipCoord.xyz/lightSpaceClipCoord.w;
//
//	//2 找到当前像素在ShadowMap上的uv
//	vec3 projCoord = lightNDC * 0.5 + 0.5;
//	vec2 uv = projCoord.xy;
//	float depth = projCoord.z;
//
//	vec2 texelSize = 1.0 / textureSize(shadowMapSampler, 0);
//
//	//3  遍历九宫格，每一个的深度值都需要与当前像素在光源下的深度值进行对比
//	float sum = 0.0;
//	for(int x = -1;x <= 1;x++){
//		for(int y = -1;y <= 1;y++){
//			float closestDepth = texture(shadowMapSampler,uv + vec2(x,y) * texelSize).r;
//			sum += closestDepth < (depth - getBias(normal, lightDir))?1.0:0.0;
//		}
//	}
//	return sum / 9.0;
//}

uniform float pcfRadius;//采样半径 对需要添加的uv做缩放
float pcf(vec3 normal, vec3 lightDir){
	//1 找到当前像素在光源空间内的NDC坐标
	vec3 lightNDC = lightSpaceClipCoord.xyz/lightSpaceClipCoord.w;

	//2 找到当前像素在ShadowMap上的uv
	vec3 projCoord = lightNDC * 0.5 + 0.5;
	vec2 uv = projCoord.xy;
	float depth = projCoord.z;

	poissonDiskSamples(uv);

	vec2 texelSize = 1.0 / textureSize(shadowMapSampler, 0);

	//3  遍历poisson采样盘的每一个采样点，每一个的深度值都需要与当前像素在光源下的深度值进行对比
	float sum = 0.0;
	for(int i = 0;i < NUM_SAMPLES;i++){
		float closestDepth = texture(shadowMapSampler,uv + disk[i] * pcfRadius).r;
		sum += closestDepth < (depth - getBias(normal, lightDir))?1.0:0.0;
	}

	return sum / float(NUM_SAMPLES);
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

	float shadow = pcf(normal, -directionalLight.direction);
	vec3 finalColor = result * (1.0 - shadow) + ambientColor;
	

	FragColor = vec4(finalColor,alpha * opacity);
}