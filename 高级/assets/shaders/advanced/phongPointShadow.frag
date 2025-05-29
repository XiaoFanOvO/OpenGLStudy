#version 460 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D sampler;	//diffuse贴图采样器

uniform samplerCube shadowMapSampler;
uniform float bias;

uniform vec3 ambientColor;

//相机世界位置
uniform vec3 cameraPosition;


uniform float shiness;

//透明度
uniform float opacity;


#include "../common/commonLight.glsl"

uniform PointLight pointLight;

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
vec3 disk[NUM_SAMPLES];
void poissonDiskSamples(vec2 randomSeed){
	//1 初始弧度
	float angleTheta = rand_2to1(randomSeed) * PI2;
	float angleFi = rand_2to1(randomSeed) * PI2;

	//2 初始半径
	float radius = 1.0 / float(NUM_SAMPLES);

	//3 弧度步长
	float angleStep = 3.883222077450933;

	//4 半径步长
	float radiusStep = radius;

	//5 循环生成
	for(int i = 0;i < NUM_SAMPLES;i++){
		disk[i] = vec3(sin(angleTheta)*cos(angleFi), cos(angleTheta),sin(angleTheta) * sin(angleFi)) * pow(radius, diskTightness);
		radius += radiusStep;
		angleTheta += angleStep;
		angleFi += angleStep;
	}
}

float getBias(vec3 normal, vec3 lightDir){
	vec3 normalN = normalize(normal);
	vec3 lightDirN = normalize(lightDir);
	
	return max(bias * (1.0 - dot(normalN, lightDirN)), 0.0005);
}


uniform float far;
uniform float pcfRadius;
float pcf(vec3 normal, vec3 lightDir,float pcfUVRadius){
	vec3 lightDis = worldPosition - pointLight.position;
	vec3 uvw = normalize(lightDis);
	float depth = length(lightDis) / (1.414*far);

	poissonDiskSamples(uvw.xy);

	//3  遍历poisson采样盘的每一个采样点，每一个的深度值都需要与当前像素在光源下的深度值进行对比
	float sum = 0.0;
	for(int i = 0;i < NUM_SAMPLES;i++){
		float closestDepth = texture(shadowMapSampler,uvw + disk[i] * pcfUVRadius).r;
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

	result += calculatePointLight(objectColor, pointLight, normalN ,viewDir); 

	float alpha =  texture(sampler, uv).a;

	vec3 ambientColor = objectColor * ambientColor;

	vec3 lightDirN = normalize(worldPosition - pointLight.position);
	float shadow = pcf(normalN, -lightDirN,pcfRadius);

	vec3 finalColor = result * (1.0 - shadow) + ambientColor;

	FragColor = vec4(finalColor,alpha * opacity);
}