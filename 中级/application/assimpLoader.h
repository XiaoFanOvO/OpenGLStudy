#pragma once
#include "../glframework/core.h"
#include "../glframework/object.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "../glframework/tools/tools.h"
#include "../glframework/mesh.h"
#include "../glframework/material/phongMaterial.h"

class AssimpLoader
{
public:
	AssimpLoader();
	~AssimpLoader();
	
	static Object* load(const std::string& path);

private:
	static void processNode(aiNode* ainode, Object* parent, const aiScene* scene);
	static Mesh* processMesh(aiMesh* aimesh);

	static glm::mat4 getMat4f(aiMatrix4x4 value);
};

