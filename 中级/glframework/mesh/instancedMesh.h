#pragma once
#include "mesh.h"


class InstancedMesh :public Mesh
{
public:
	InstancedMesh(Geometry* geometry, Material* material, unsigned int instanceCount);
	~InstancedMesh();

	unsigned int mInstanceCount{ 0 };
	glm::mat4* mInstanceMatrices{ nullptr };
	unsigned int mMatrixVbo{ 0 };
	void updateMatrices();
};

