#pragma once
#include "mesh.h"
#include <algorithm>


class InstancedMesh :public Mesh
{
public:
	InstancedMesh(Geometry* geometry, Material* material, unsigned int instanceCount);
	InstancedMesh(Geometry* geometry, Material* material, unsigned int instanceCount, glm::mat4 T0, glm::mat4 T1, glm::mat4 T2);
	~InstancedMesh();

	unsigned int mInstanceCount{ 0 };
	std::vector<glm::mat4> mInstanceMatrices{};
	unsigned int mMatrixVbo{ 0 };
	void updateMatrices();
	void sortMatrices(glm::mat4 viewMatrix);
};

