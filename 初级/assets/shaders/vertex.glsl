#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
uniform float time;
uniform float speed;
out vec3 color;
out vec2 uv;

uniform mat4 transform;//�ⲿ����任����
uniform mat4 viewMatrix;//��ͼ����
uniform mat4 projectionMatrix;//ͶӰ����


//aPos��Ϊattribute(����)����shader ���������
void main()
{
	//gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	//gl_Position = vec4(sin(time) * aPos.x, aPos.y, aPos.z, 1.0);
	//gl_Position = vec4(aPos.x + sin(time * speed) * 0.5f, aPos.y, aPos.z, 1.0);
	//color = aColor * (cos(time) + 1.0f) / 2.0f;
	//color = aColor;
	//uv = aUV;

	//�ֶ�mipmapʵ��
	//1 ��ǰ�����εĶ���  ���ŵı���
	//float scale = 1.0 / time;
	//2 ʹ��scale�Զ���λ�ý�������
	//vec3 sPos = aPos * scale;
	//3 �����λ����Ϣ
	//gl_Position = vec4(sPos, 1.0);
	//color = aColor;
	//uv = aUV;

	vec4 position = vec4(aPos, 1.0);
	position = projectionMatrix * viewMatrix * transform * position;
	gl_Position = position;
	uv = aUV;
}