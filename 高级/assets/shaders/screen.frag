#version 460 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D screenTexSampler;

uniform float texWidth;
uniform float texHeight;

//颜色反相
vec3 colorInvert(vec3 color){
	vec3 invertColor = vec3(1.0) - color;
	return invertColor;
}

//平均灰度
vec3 gray(vec3 color){
	float avg = (color.r + color.g +color.b) / 3.0;
	return vec3(avg);
}

//由于人眼对于绿色更加敏感，对蓝色不敏感，为了物理精确
//可以为每个颜色通道，增加权重值，让蓝色小一些，绿色多一些
vec3 grayCorrect(vec3 color){
	float avg = color.r * 0.2126 + color.g * 0.7152 +color.b * 0.0722;
	return vec3(avg);
}


vec3 blur(){
	float du = 1.0 / texWidth;
	float dv = 1.0 / texHeight;

	//偏移值数组
	vec2 offsets[9]=vec2[](
		vec2(-du, dv),//左上
		vec2(0.0, dv),//正上
		vec2(du, dv),//右上
		vec2(-du, 0.0),//左
		vec2(0.0, 0.0),//中
		vec2(du, 0.0),//右
		vec2(-du, -dv),//左下
		vec2(0.0, -dv),//正下
		vec2(du, -dv)//右下
	);

	//卷积核
	float kernel[9] = float[](
		1.0, 2.0, 1.0,
		2.0, 4.0, 2.0,
		1.0, 2.0, 1.0
	);

	//加权相加
	vec3 sumColor = vec3(0.0);
	for(int i = 0;i < 9;i++){
		vec3 sampleColor = texture(screenTexSampler, uv + offsets[i]).rgb;
		sumColor += sampleColor * kernel[i];
	}
	sumColor /= 16.0;

	return sumColor;
}

void main()
{
	//vec3 color = colorInvert(texture(screenTexSampler, uv).rgb);
	//vec3 color = gray(texture(screenTexSampler, uv).rgb);
	//vec3 color = grayCorrect(texture(screenTexSampler, uv).rgb);
//	vec3 color = texture(screenTexSampler, uv).rgb;
//	vec3 color = blur();

	vec3 color = texture(screenTexSampler, uv).rgb;

	//2 最终颜色要抵抗屏幕gamma
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);
}