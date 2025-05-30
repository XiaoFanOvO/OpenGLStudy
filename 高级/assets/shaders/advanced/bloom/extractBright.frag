#version 460 core
out vec4 FragColor;

//本shader的目的是提取纹理中的亮部
in vec2 uv;

uniform sampler2D srcTex;//HDR的texture(颜色可能超过1)
uniform float threshold;

void main()
{
	vec3 color = texture(srcTex, uv).rgb;

	float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

	if(brightness > threshold){
		FragColor = vec4(color, 1.0);
	}else{
		FragColor = vec4(0.0,0.0,0.0,0.0);
	}

}