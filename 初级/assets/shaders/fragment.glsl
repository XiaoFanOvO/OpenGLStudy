#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 uv;
uniform float time;
uniform vec3 uColor;
uniform sampler2D sampler;
uniform sampler2D grassSampler;
uniform sampler2D landSampler;
uniform sampler2D noiseSampler;

//外部传入纹理的宽高
uniform float width;
uniform float height;

void main()
{
	//float intensity = (sin(time) + 1.0f) / 2.0f;
	//FragColor = vec4(vec3(intensity) + color, 1.0f);
	//FragColor = vec4(color, 1.0f);
	//FragColor = texture(sampler, uv);


	//纹理混合实验
	//vec4 grassColor = texture(grassSampler, uv);
	//vec4 landColor = texture(landSampler, uv);
	//vec4 noiseColor = texture(noiseSampler, uv);

	//float weight = noiseColor.r;
	//vec4 finalColor = grassColor * weight + landColor * (1.0 - weight);
	//vec4 finalColor = mix(grassColor, landColor, weight);//weight只指第二个参数也就是landColor的权重

	//FragColor = vec4(finalColor.xyz, 1.0f);

	//mipmap实验
	//1 获取当前像素对应的纹理上的纹素具体位置
	//vec2 location = uv * vec2(width, height);
	//2 计算当前像素对应纹素具体位置在xy方向上的变化量
	//vec2 dx = dFdx(location);
	//vec2 dy = dFdy(location);
	//3 选择最大的delta  求log2(delta)
	//float maxDelta = sqrt(max(dot(dx,dx),dot(dy,dy)));
	//float L = log2(maxDelta);//初步计算出来的级别

	//4 计算出mipmap的采样级别
	//int level = max(int(L + 0.5), 0);

	//FragColor = textureLod(sampler, uv, level);
	FragColor = texture(sampler, uv);
}