#pragma once
#include "../glframework/core.h"
#include "../glframework/object.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "../glframework/tools/tools.h"
#include "../glframework/mesh/mesh.h"
#include "../glframework/material/phongMaterial.h"
#include "../glframework/material/phongInstanceMaterial.h"
#include "../glframework/material/grassInstanceMaterial.h"
#include "../glframework/texture.h"
#include "../glframework/mesh/instancedMesh.h"

class AssimpInstanceLoader
{
public:
	AssimpInstanceLoader();
	~AssimpInstanceLoader();
	
	static Object* load(const std::string& path, int instanceCount);

private:
	static void processNode(aiNode* ainode, Object* parent, const aiScene* scene, const std::string& rootPath, int instanceCount);
	static InstancedMesh* processMesh(aiMesh* aimesh, const aiScene* scene, const std::string& rootPath, int instanceCount);
	static Texture* processTexture(const aiMaterial* aimat, const aiTextureType& type, const aiScene* scene, const std::string& rootPath);

	static glm::mat4 getMat4f(aiMatrix4x4 value);
};

