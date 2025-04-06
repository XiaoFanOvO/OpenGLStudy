#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;//�����������worldPosition GPU���Զ����в�ֵ����

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat3 normalMatrix;

//aPos��Ϊattribute�����ԣ�����shader
//��������ĵ�
void main()
{
	//������Ķ���λ��ת��Ϊ�������(3ά-4ά)
	vec4 transformPosition = vec4(aPos, 1.0);
	//��һ���м����TransformPosition,���ڼ�����άλ����modelMatrix��˵��м���
	transformPosition = modelMatrix * transformPosition;
	//���㵱ǰ�����worldPosition ����������FragmentShader
	worldPosition = transformPosition.xyz;
	gl_Position = projectionMatrix * viewMatrix * transformPosition;
	uv = aUV;
	//��shader�м��㷨�ߵı任����ǳ���������(��Ҫ������任) һ������CPU�˼���ͨ��uniform��������
	normal = normalMatrix * aNormal;
}