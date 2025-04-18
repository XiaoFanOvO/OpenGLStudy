#include "instancedMesh.h"

InstancedMesh::InstancedMesh(
	Geometry* geometry, 
	Material* material, 
	unsigned int instanceCount):Mesh(geometry, material) {  //调用父类的有参构造函数
	mType = ObjectType::InstancedMesh;
	mInstanceCount = instanceCount;
	mInstanceMatrices = new glm::mat4[instanceCount];//这个时候这是个空数组，先占位，后面会单独灌数据

	glGenBuffers(1, &mMatrixVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices, GL_DYNAMIC_DRAW); //支持动态改变
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
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices);//这个才是专门用来更新VBO数据的函数
	glBindVertexArray(0);
}

InstancedMesh::~InstancedMesh() {
	if (mInstanceMatrices != nullptr)
	{
		delete[]mInstanceMatrices;
	}
}	