#include "instancedMesh.h"

InstancedMesh::InstancedMesh(
	Geometry* geometry, 
	Material* material, 
	unsigned int instanceCount):Mesh(geometry, material) {  //���ø�����вι��캯��
	mType = ObjectType::InstancedMesh;
	mInstanceCount = instanceCount;
	//mInstanceMatrices = new glm::mat4[instanceCount];//���ʱ�����Ǹ������飬��ռλ������ᵥ��������
	mInstanceMatrices.resize(instanceCount);//Ԥ���ռ�

	glGenBuffers(1, &mMatrixVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices.data(), GL_DYNAMIC_DRAW); //֧�ֶ�̬�ı�
	glBindVertexArray(mGeometry->getVao());
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(4 + i);//����ԭ������λ�÷��˲ݵĶ�����ɫ
		glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * i * 4));
		glVertexAttribDivisor(4 + i, 1);//��ʵ������
	}
	glBindVertexArray(0);
}


InstancedMesh::InstancedMesh(
	Geometry* geometry,
	Material* material,
	unsigned int instanceCount, glm::mat4 T0, glm::mat4 T1, glm::mat4 T2) :Mesh(geometry, material) {  //���ø�����вι��캯��
	mType = ObjectType::InstancedMesh;
	mInstanceCount = instanceCount;
	//mInstanceMatrices = new glm::mat4[instanceCount];//���ʱ�����Ǹ������飬��ռλ������ᵥ��������
	mInstanceMatrices.resize(instanceCount);
	mInstanceMatrices[0] = T0;
	mInstanceMatrices[1] = T1;
	mInstanceMatrices[2] = T2;

	glGenBuffers(1, &mMatrixVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mMatrixVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices.data(), GL_DYNAMIC_DRAW); //֧�ֶ�̬�ı�
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
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * mInstanceCount, mInstanceMatrices.data());//�������ר����������VBO���ݵĺ���
	glBindVertexArray(0);
}

void InstancedMesh::sortMatrices(glm::mat4 viewMatrix) {
	std::sort(
		mInstanceMatrices.begin(),
		mInstanceMatrices.end(),
		[viewMatrix](const glm::mat4& a, const glm::mat4& b) {
			//1 ����A�����ϵ��Z
			auto modelMatrixA = a;
			auto worldPositionA = modelMatrixA * glm::vec4(0.0, 0.0, 0.0, 1.0);//���е���Ƭ�ʼ������ԭ���
			//��������ģ�ͱ任�����������ռ������
			auto cameraPositionA = viewMatrix * worldPositionA;

			//2 ����B�����ϵ��Z
			auto modelMatrixB = b;
			auto worldPositionB = modelMatrixB * glm::vec4(0.0, 0.0, 0.0, 1.0);//���е���Ƭ�ʼ������ԭ���
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;
		});
}

InstancedMesh::~InstancedMesh() {

}	