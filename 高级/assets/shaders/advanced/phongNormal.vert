#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;
out mat3 tbn;//矩阵也是可以通过插值给到每个像素点的

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat3 normalMatrix;

//aPos作为attribute（属性）传入shader
//不允许更改的
void main()
{
// 将输入的顶点位置，转化为齐次坐标（3维-4维）
	vec4 transformPosition = vec4(aPos, 1.0);

	//做一个中间变量TransformPosition，用于计算四位位置与modelMatrix相乘的中间结果
	transformPosition = modelMatrix * transformPosition;

	//计算当前顶点的worldPosition，并且向后传输给FragmentShader
	worldPosition = transformPosition.xyz;

	gl_Position = projectionMatrix * viewMatrix * transformPosition;
	
	uv = aUV;
//	normal =  transpose(inverse(mat3(modelMatrix))) * aNormal;
	normal =  normalMatrix * aNormal;//世界空间的法线
	vec3 tangent = normalize(mat3(modelMatrix) * aTangent);//获得世界空间的切线
	vec3 bitangent = normalize(cross(normal, tangent)); //世界空间的副切线
	tbn = mat3(tangent, bitangent, normal);//构建tbn矩阵(用于法线贴图的转换 从tbn空间-世界空间)
}