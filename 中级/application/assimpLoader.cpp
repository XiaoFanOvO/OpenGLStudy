#include "assimpLoader.h"

AssimpLoader::AssimpLoader()
{
}

AssimpLoader::~AssimpLoader()
{
}

Object* AssimpLoader::load(const std::string& path) {
	Object* rootNode = new Object();
	Assimp::Importer importer; //������
	//aiProcess_Triangulate ��ģ���е��ı��������λ�
	//aiProcess_GenNormals  ���ģ��û�з��������ɷ���
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

	//��֤��ȡ�Ƿ���ȷ˳��
	//AI_SCENE_FLAGS_INCOMPLETE �ļ�������
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene -> mRootNode)
	{
		std::cerr << "Error: Model Read Failed!" << std::endl;
		return nullptr;
	}

	processNode(scene->mRootNode, rootNode, scene);

	return rootNode;
}

//����ڵ�
//aiNode������children����Ϣ,Ҳ��mesh�ı��
//ÿ���ڵ��Ӧһ��object
void AssimpLoader::processNode(aiNode* ainode, Object* parent, const aiScene* scene) {
	Object* node = new Object();
	parent->addChild(node);//����Object�ĸ��ӹ�ϵ
	glm::mat4 localMatrix = getMat4f(ainode->mTransformation);
	//λ�� ��ת ����
	glm::vec3 position, eulerAngle, scale;
	Tools::decompose(localMatrix, position, eulerAngle, scale);
	node->setPosition(position);
	node->setAngleX(eulerAngle.x);
	node->setAngleY(eulerAngle.y);
	node->setAngleZ(eulerAngle.z);
	node->setScale(scale); 

	//�����û��mesh���ҽ���
	for (int i = 0; i < ainode->mNumMeshes; i++)
	{
		int meshID = ainode->mMeshes[i]; //����ֻ��֪��ID Ҫ�ҵ�mesh�Ķ��󻹵�ȥ����aiScene����
		aiMesh* aiMesh = scene->mMeshes[meshID];//ainodeֻ�Ǵ��ID scene������Ǵ�Ķ���
		Mesh* mesh = processMesh(aiMesh);
		node->addChild(mesh); //object���治ֻ��ҽڵ�node �����mesh
	}

	for (int i = 0; i < ainode->mNumChildren; i++)
	{
		processNode(ainode->mChildren[i], node, scene);
	}

}

//����mesh
Mesh* AssimpLoader::processMesh(aiMesh* aimesh) {
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;

	for (int i = 0; i < aimesh->mNumVertices; i++)
	{
		//��i�������λ��
		positions.push_back(aimesh->mVertices[i].x);
		positions.push_back(aimesh->mVertices[i].y);
		positions.push_back(aimesh->mVertices[i].z);
		
		//��i������ķ���
		normals.push_back(aimesh->mNormals[i].x);
		normals.push_back(aimesh->mNormals[i].y);
		normals.push_back(aimesh->mNormals[i].z);

		//��i�������uv
		//һ��mesh������n��uv(diffuse��ͼuv,������ͼuv...)
		//һ���ע��0��uv,һ������µ�0��uv����ͼuv
		if (aimesh->mTextureCoords[0]) //������ڵ�0��uv
		{
			uvs.push_back(aimesh->mTextureCoords[0][i].x); //u
			uvs.push_back(aimesh->mTextureCoords[0][i].y); //v
		}
		else 
		{
			//��������ڵ�0��uv ������ΪĬ��ֵ
			uvs.push_back(0.0f);
			uvs.push_back(0.0f);
		}
	}

	//����mesh�е�����ֵ
	//ÿ�������ξ���һ��aiFace
	//����ÿһ��������(face) -> ����ÿһ��indices
	for (int i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	auto geometry = new Geometry(positions, normals, uvs, indices);
	auto material = new PhongMaterial();
	material->mDiffuse = new Texture("assets/textures/box.png", 0);

	return new Mesh(geometry, material);
}



//��Assimp��aiMatrixת��ΪopenGL��glmat4
glm::mat4 AssimpLoader::getMat4f(aiMatrix4x4 value) {
	glm::mat4 to(
		value.a1, value.a2, value.a3, value.a4,
		value.b1, value.b2, value.b3, value.b4,
		value.c1, value.c2, value.c3, value.c4,
		value.d1, value.d2, value.d3, value.d4
	);

	return to;
}