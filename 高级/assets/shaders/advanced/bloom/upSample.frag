#version 460 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D lowerResTex;//被上采样的低分辨率图像
uniform sampler2D higherResTex;//来自下采样链条,与当前绘制目标大小相同的纹理

#define NUM_SAMPLES 32
#define PI 3.141592653589793
#define PI2 6.283185307179586

float rand_2to1(vec2 uv ) { 
  // 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}
vec2 disk[NUM_SAMPLES];
void poissonDiskSamples(vec2 randomSeed){
	//1 初始弧度
	float angle = rand_2to1(randomSeed) * PI2;

	//2 初始半径
	float radius = 1.0 / float(NUM_SAMPLES);

	//3 弧度步长
	float angleStep = 3.883222077450933;

	//4 半径步长
	float radiusStep = radius;

	//5 循环生成
	for(int i = 0;i < NUM_SAMPLES;i++){
		disk[i] = vec2(cos(angle), sin(angle)) * radius;
		radius += radiusStep;
		angle += angleStep;
	}
}


uniform float bloomRadius;
vec3 doPoissonSample(sampler2D tex, vec2 uv){
	poissonDiskSamples(uv);
	vec3 sumColor = vec3(0.0);
	for(int i = 0; i < NUM_SAMPLES; i++){
		sumColor += texture(tex, uv + disk[i] * bloomRadius).rgb;
	}
	return sumColor / NUM_SAMPLES;
}

uniform float bloomAttenuation;
void main()
{
	vec3 lowerResColor = doPoissonSample(lowerResTex, uv);
	vec3 higherResColor = doPoissonSample(higherResTex, uv);
	
	vec3 color = higherResColor + lowerResColor * bloomAttenuation;
	FragColor = vec4(color, 1.0);

}