#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
uniform float time;
uniform float speed;
out vec3 color;
out vec2 uv;

uniform mat4 transform;//外部传入变换矩阵
uniform mat4 viewMatrix;//视图矩阵
uniform mat4 projectionMatrix;//投影矩阵


//aPos作为attribute(属性)传入shader 不允许更改
void main()
{
	//gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	//gl_Position = vec4(sin(time) * aPos.x, aPos.y, aPos.z, 1.0);
	//gl_Position = vec4(aPos.x + sin(time * speed) * 0.5f, aPos.y, aPos.z, 1.0);
	//color = aColor * (cos(time) + 1.0f) / 2.0f;
	//color = aColor;
	//uv = aUV;

	//手动mipmap实验
	//1 当前三角形的顶点  缩放的比例
	//float scale = 1.0 / time;
	//2 使用scale对顶点位置进行缩放
	//vec3 sPos = aPos * scale;
	//3 向后传输位置信息
	//gl_Position = vec4(sPos, 1.0);
	//color = aColor;
	//uv = aUV;

	vec4 position = vec4(aPos, 1.0);
	position = projectionMatrix * viewMatrix * transform * position;
	gl_Position = position;
	uv = aUV;
}