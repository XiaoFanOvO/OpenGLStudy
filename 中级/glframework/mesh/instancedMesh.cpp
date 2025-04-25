#include "instancedMesh.h"

InstancedMesh::InstancedMesh(
	Geometry* geometry, 
	Material* material, 
	unsigned int instanceCount):Mesh(geometry, material) {  //调用父类的有参构造函数
	mType = ObjectType::InstancedMesh;
	mInstanceCount = instanceCount;
	//mInstanceMatrices = new glm::mat4[instanceCount];//这个时候这是个空数组，先占位，后面会单独灌数据
	mInstanceMatrices.resize(instanceCount);//预留空间

	glGenBuffers(1, &mMatrixVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices.data(), GL_DYNAMIC_DRAW); //支持动态改变
	glBindVertexArray(mGeometry->getVao());
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(4 + i);//这里原来三号位置放了草的顶点颜色
		glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * i * 4));
		glVertexAttribDivisor(4 + i, 1);//逐实例更新
	}
	glBindVertexArray(0);
}


InstancedMesh::InstancedMesh(
	Geometry* geometry,
	Material* material,
	unsigned int instanceCount, glm::mat4 T0, glm::mat4 T1, glm::mat4 T2) :Mesh(geometry, material) {  //调用父类的有参构造函数
	mType = ObjectType::InstancedMesh;
	mInstanceCount = instanceCount;
	//mInstanceMatrices = new glm::mat4[instanceCount];//这个时候这是个空数组，先占位，后面会单独灌数据
	mInstanceMatrices.resize(instanceCount);
	mInstanceMatrices[0] = T0;
	mInstanceMatrices[1] = T1;
	mInstanceMatrices[2] = T2;

	glGenBuffers(1, &mMatrixVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices.data(), GL_DYNAMIC_DRAW); //支持动态改变
	glBindVertexArray(mGeometry->getVao());
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(3 + i);
		glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * i * 4));
		glVertexAttribDivisor(3 + i, 1);//逐实例更新
	}
	glBindVertexArray(0);
}




void InstancedMesh::updateMatrices() {
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	//如果使用glBufferData进行数据更新，会导致重新分配显存空间
	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices.data());//这个才是专门用来更新VBO数据的函数
	glBindVertexArray(0);
}

void InstancedMesh::sortMatrices(glm::mat4 viewMatrix) {
	std::sort(
		mInstanceMatrices.begin(),
		mInstanceMatrices.end(),
		[viewMatrix](const glm::mat4& a, const glm::mat4& b) {
			//1 计算A的相机系的Z
			auto modelMatrixA = a;
			auto worldPositionA = modelMatrixA * glm::vec4(0.0, 0.0, 0.0, 1.0);//所有的面片最开始都是在原点的
			//乘上他的模型变换矩阵就是世界空间的坐标
			auto cameraPositionA = viewMatrix * worldPositionA;

			//2 计算B的相机系的Z
			auto modelMatrixB = b;
			auto worldPositionB = modelMatrixB * glm::vec4(0.0, 0.0, 0.0, 1.0);//所有的面片最开始都是在原点的
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;
		});
}

InstancedMesh::~InstancedMesh() {

}	