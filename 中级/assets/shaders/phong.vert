#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;//这里算出来的worldPosition GPU会自动进行插值处理

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat3 normalMatrix;

//aPos作为attribute（属性）传入shader
//不允许更改的
void main()
{
	//将输入的顶点位置转化为其次坐标(3维-4维)
	vec4 transformPosition = vec4(aPos, 1.0);
	//做一个中间变量TransformPosition,用于计算四维位置与modelMatrix相乘的中间结果
	transformPosition = modelMatrix * transformPosition;
	//计算当前顶点的worldPosition 并且向后传输给FragmentShader
	worldPosition = transformPosition.xyz;
	gl_Position = projectionMatrix * viewMatrix * transformPosition;
	uv = aUV;
	//在shader中计算法线的变换矩阵非常消耗性能(主要是求逆变换) 一般是在CPU端计算通过uniform变量传入
	normal = normalMatrix * aNormal;
}