#include"assimpInstanceLoader.h"
#include "../glframework/tools/tools.h"
#include "../glframework/material/phongInstanceMaterial.h"

Object* AssimpInstanceLoader::load(const std::string& path, int instanceCount) {
	//�ó�ģ������Ŀ¼
	std::size_t lastIndex = path.find_last_of("//");
	auto rootPath = path.substr(0, lastIndex + 1);

	Object* rootNode = new Object();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

	//��֤��ȡ�Ƿ���ȷ˳��
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cerr << "Error:Model Read Failed!" << std::endl;
		return nullptr;
	}

	processNode(scene->mRootNode, rootNode, scene, rootPath,instanceCount);
	
	return rootNode;
}

void AssimpInstanceLoader::processNode(aiNode* ainode, Object* parent, const aiScene* scene, const std::string& rootPath, int instanceCount) {
	Object* node = new Object();
	parent->addChild(node);

	glm::mat4 localMatrix = getMat4f(ainode->mTransformation);
	//λ�� ��ת ����
	glm::vec3 position, eulerAngle, scale;
	Tools::decompose(localMatrix, position, eulerAngle, scale);
	node->setPosition(position);
	node->setAngleX(eulerAngle.x);
	node->setAngleY(eulerAngle.y);
	node->setAngleZ(eulerAngle.z);
	node->setScale(scale);

	//�����û��mesh�����ҽ���
	for (int i = 0; i < ainode->mNumMeshes; i++) {
		int meshID = ainode->mMeshes[i];
		aiMesh* aimesh = scene->mMeshes[meshID];
		auto mesh = processMesh(aimesh, scene, rootPath,instanceCount);
		node->addChild(mesh);
	}

	for (int i = 0; i < ainode->mNumChildren; i++) {
		processNode(ainode->mChildren[i], node, scene, rootPath,instanceCount);
	}
}

InstancedMesh* AssimpInstanceLoader::processMesh(aiMesh* aimesh, const aiScene* scene, const std::string& rootPath, int instanceCount) {
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<float> colors;

	std::vector<unsigned int> indices;

	for (int i = 0; i < aimesh->mNumVertices; i++) {
		//��i�������λ��
		positions.push_back(aimesh->mVertices[i].x);
		positions.push_back(aimesh->mVertices[i].y);
		positions.push_back(aimesh->mVertices[i].z);

		//��i������ķ���
		normals.push_back(aimesh->mNormals[i].x);
		normals.push_back(aimesh->mNormals[i].y);
		normals.push_back(aimesh->mNormals[i].z);

		//��i���������ɫ
		if (aimesh->HasVertexColors(0)) {
			colors.push_back(aimesh->mColors[0][i].r);
			colors.push_back(aimesh->mColors[0][i].g);
			colors.push_back(aimesh->mColors[0][i].b);
		}

		//��i�������uv
		//��ע���0��uv��һ�����������ͼuv
		if (aimesh->mTextureCoords[0]) {
			uvs.push_back(aimesh->mTextureCoords[0][i].x);
			uvs.push_back(aimesh->mTextureCoords[0][i].y);
		}
		else {
			uvs.push_back(0.0f);
			uvs.push_back(0.0f);
		}
	}

	//����mesh�е�����ֵ
	for (int i = 0; i < aimesh->mNumFaces; i++) {
		aiFace face = aimesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	auto geometry = new Geometry(positions, normals, uvs,colors, indices);
	auto material = new PhongInstanceMaterial();

	if (aimesh->mMaterialIndex >= 0) {
		Texture* texture = nullptr;
		aiMaterial* aiMat = scene->mMaterials[aimesh->mMaterialIndex];
		texture = processTexture(aiMat, aiTextureType_DIFFUSE, scene, rootPath);
		if (texture == nullptr) {
			texture = Texture::createTexture("assets/textures/defaultTexture.jpg", 0);
		}
		material->mDiffuse = texture;
	}
	else {
		material->mDiffuse = Texture::createTexture("assets/textures/defaultTexture.jpg", 0);
	}
	
	return new InstancedMesh(geometry ,material,instanceCount);
}


Texture* AssimpInstanceLoader::processTexture(
	const aiMaterial* aimat,
	const aiTextureType& type,
	const aiScene* scene,
	const std::string& rootPath
) {
	Texture* texture = nullptr;
	//��ȡͼƬ��ȡ·��
	aiString aipath;
	aimat->Get(AI_MATKEY_TEXTURE(type, 0), aipath);
	if (!aipath.length) {
		return nullptr;
	}

	//�ж��Ƿ���Ƕ��fbx��ͼƬ
	const aiTexture* aitexture = scene->GetEmbeddedTexture(aipath.C_Str());
	if (aitexture) {
		//����ͼƬ����Ƕ��
		unsigned char* dataIn = reinterpret_cast<unsigned char*>(aitexture->pcData);
		uint32_t widthIn = aitexture->mWidth;//ͨ������£�png��jpg��������������ͼƬ��С
		uint32_t heightIn = aitexture->mHeight;
		texture = Texture::createTextureFromMemory(aipath.C_Str(), 0, dataIn, widthIn, heightIn);
	}
	else {
		//����ͼƬ��Ӳ����
		std::string fullPath = rootPath + aipath.C_Str();
		texture = Texture::createTexture(fullPath, 0);
	}

	return texture;
}



glm::mat4 AssimpInstanceLoader::getMat4f(aiMatrix4x4 value) {
	glm::mat4 to(
		value.a1, value.a2, value.a3, value.a4,
		value.b1, value.b2, value.b3, value.b4,
		value.c1, value.c2, value.c3, value.c4,
		value.d1, value.d2, value.d3, value.d4
	);

	return to;
}
