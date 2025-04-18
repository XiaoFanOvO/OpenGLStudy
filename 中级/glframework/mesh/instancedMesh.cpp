#include "instancedMesh.h"

InstancedMesh::InstancedMesh(
	Geometry* geometry, 
	Material* material, 
	unsigned int instanceCount):Mesh(geometry, material) {  //���ø�����вι��캯��
	mType = ObjectType::InstancedMesh;
	mInstanceCount = instanceCount;
	mInstanceMatrices = new glm::mat4[instanceCount];//���ʱ�����Ǹ������飬��ռλ������ᵥ��������

	glGenBuffers(1, &mMatrixVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices, GL_DYNAMIC_DRAW); //֧�ֶ�̬�ı�
	glBindVertexArray(mGeometry->getVao());
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(3 + i);
		glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * i * 4));
		glVertexAttribDivisor(3 + i, 1);//��ʵ������
	}
	glBindVertexArray(0);
}

void InstancedMesh::updateMatrices() {
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	//���ʹ��glBufferData�������ݸ��£��ᵼ�����·����Դ�ռ�
	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices);//�������ר����������VBO���ݵĺ���
	glBindVertexArray(0);
}

InstancedMesh::~InstancedMesh() {
	if (mInstanceMatrices != nullptr)
	{
		delete[]mInstanceMatrices;
	}
}	