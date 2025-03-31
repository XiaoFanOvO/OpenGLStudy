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

//�ⲿ��������Ŀ��
uniform float width;
uniform float height;

void main()
{
	//float intensity = (sin(time) + 1.0f) / 2.0f;
	//FragColor = vec4(vec3(intensity) + color, 1.0f);
	//FragColor = vec4(color, 1.0f);
	//FragColor = texture(sampler, uv);


	//������ʵ��
	//vec4 grassColor = texture(grassSampler, uv);
	//vec4 landColor = texture(landSampler, uv);
	//vec4 noiseColor = texture(noiseSampler, uv);

	//float weight = noiseColor.r;
	//vec4 finalColor = grassColor * weight + landColor * (1.0 - weight);
	//vec4 finalColor = mix(grassColor, landColor, weight);//weightָֻ�ڶ�������Ҳ����landColor��Ȩ��

	//FragColor = vec4(finalColor.xyz, 1.0f);

	//mipmapʵ��
	//1 ��ȡ��ǰ���ض�Ӧ�������ϵ����ؾ���λ��
	//vec2 location = uv * vec2(width, height);
	//2 ���㵱ǰ���ض�Ӧ���ؾ���λ����xy�����ϵı仯��
	//vec2 dx = dFdx(location);
	//vec2 dy = dFdy(location);
	//3 ѡ������delta  ��log2(delta)
	//float maxDelta = sqrt(max(dot(dx,dx),dot(dy,dy)));
	//float L = log2(maxDelta);//������������ļ���

	//4 �����mipmap�Ĳ�������
	//int level = max(int(L + 0.5), 0);

	//FragColor = textureLod(sampler, uv, level);
	FragColor = texture(sampler, uv);
}