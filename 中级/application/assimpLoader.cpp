#include "assimpLoader.h"

AssimpLoader::AssimpLoader()
{
}

AssimpLoader::~AssimpLoader()
{
}

Object* AssimpLoader::load(const std::string& path) {
	//拿出模型所在目录
	std::size_t lastIndex = path.find_last_of("//");
	auto rootPath = path.substr(0, lastIndex + 1);


	Object* rootNode = new Object();
	Assimp::Importer importer; //导入器
	//aiProcess_Triangulate 将模型中的四边面三角形化
	//aiProcess_GenNormals  如果模型没有法线则生成法线
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

	//验证读取是否正确顺利
	//AI_SCENE_FLAGS_INCOMPLETE 文件不完整
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene -> mRootNode)
	{
		std::cerr << "Error: Model Read Failed!" << std::endl;
		return nullptr;
	}

	processNode(scene->mRootNode, rootNode, scene, rootPath);

	return rootNode;
}

//处理节点
//aiNode下面有children的信息,也有mesh的编号
//每个节点对应一个object
void AssimpLoader::processNode(aiNode* ainode, Object* parent, const aiScene* scene, const std::string& rootPath) {
	Object* node = new Object();
	parent->addChild(node);//建立Object的父子关系
	glm::mat4 localMatrix = getMat4f(ainode->mTransformation);
	//位置 旋转 缩放
	glm::vec3 position, eulerAngle, scale;
	Tools::decompose(localMatrix, position, eulerAngle, scale);
	node->setPosition(position);
	node->setAngleX(eulerAngle.x);
	node->setAngleY(eulerAngle.y);
	node->setAngleZ(eulerAngle.z);
	node->setScale(scale); 

	//检查有没有mesh并且解析
	for (int i = 0; i < ainode->mNumMeshes; i++)
	{
		int meshID = ainode->mMeshes[i]; //这里只是知道ID 要找到mesh的对象还得去场景aiScene里面
		aiMesh* aiMesh = scene->mMeshes[meshID];//ainode只是存的ID scene里面才是存的对象
		Mesh* mesh = processMesh(aiMesh, scene, rootPath);
		node->addChild(mesh); //object下面不只会挂节点node 还会挂mesh
	}

	for (int i = 0; i < ainode->mNumChildren; i++)
	{
		processNode(ainode->mChildren[i], node, scene, rootPath);
	}

}

//处理mesh
Mesh* AssimpLoader::processMesh(aiMesh* aimesh, const aiScene* scene, const std::string& rootPath) {
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<unsigned int> indices;

	for (int i = 0; i < aimesh->mNumVertices; i++)
	{
		//第i个定点的位置
		positions.push_back(aimesh->mVertices[i].x);
		positions.push_back(aimesh->mVertices[i].y);
		positions.push_back(aimesh->mVertices[i].z);
		
		//第i个定点的法线
		normals.push_back(aimesh->mNormals[i].x);
		normals.push_back(aimesh->mNormals[i].y);
		normals.push_back(aimesh->mNormals[i].z);

		//第i个定点的uv
		//一个mesh可能有n套uv(diffuse贴图uv,光照贴图uv...)
		//一般关注第0套uv,一般情况下第0套uv是贴图uv
		if (aimesh->mTextureCoords[0]) //如果存在第0套uv
		{
			uvs.push_back(aimesh->mTextureCoords[0][i].x); //u
			uvs.push_back(aimesh->mTextureCoords[0][i].y); //v
		}
		else 
		{
			//如果不存在第0套uv 则设置为默认值
			uvs.push_back(0.0f);
			uvs.push_back(0.0f);
		}
	}

	//解析mesh中的索引值
	//每个三角形就是一个aiFace
	//遍历每一个三角形(face) -> 遍历每一个indices
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
	//material->mDepthWrite = false;
	//进行纹理读取
	if (aimesh->mMaterialIndex >= 0)
	{
		Texture* texture = nullptr;
		aiMaterial* aiMat = scene->mMaterials[aimesh->mMaterialIndex];
		//1 读取了diffuse贴图
		texture = processTexture(aiMat, aiTextureType_DIFFUSE, scene, rootPath);
		if (texture == nullptr)
		{
			texture = Texture::createTexture("assets/textures/defaultTexture.jpg", 0);
		}
		texture->setUnit(0);
		material->mDiffuse = texture;
		//2 读取specular贴图
		auto speclarMask = processTexture(aiMat, aiTextureType_SPECULAR, scene, rootPath);
		if (speclarMask == nullptr)
		{
			speclarMask = Texture::createTexture("assets/textures/defaultTexture.jpg", 0);
		}
		speclarMask->setUnit(1);
		material->mSpecularMask = speclarMask;
	}
	else
	{
		material->mDiffuse = Texture::createTexture("assets/textures/defaultTexture.jpg", 0);
	}

	return new Mesh(geometry, material);
}


Texture* AssimpLoader::processTexture(const aiMaterial* aimat, const aiTextureType& type, const aiScene* scene, const std::string& rootPath) {
	Texture* texture = nullptr;
	//获取图片读取路径
	aiString aipath;
	aimat->Get(AI_MATKEY_TEXTURE(type, 0), aipath); //获取diffuse贴图的第0张
	if (!aipath.length)
	{
		return nullptr;
	}

	//判断是否是嵌入fbx的图片
	const aiTexture* aitexture = scene->GetEmbeddedTexture(aipath.C_Str());//判断当前路径有没有嵌入的纹理与之对应
	if (aitexture)
	{
		//纹理图片是内嵌的
		unsigned char* dataIn = reinterpret_cast<unsigned char*>(aitexture->pcData); // reinterpret_cast是强制的类型转换
		uint32_t widthIn = aitexture->mWidth; //通常情况下(png, jpg),代表了整张图片的大小(height是0)
		uint32_t heightIn = aitexture->mHeight;
		texture = Texture::createTextureFromMemory(aipath.C_Str(), 0, dataIn, widthIn, heightIn);
	}
	else
	{
		//纹理图片在硬盘上
		std::string fullPath = rootPath + aipath.C_Str();
		texture = Texture::createTexture(fullPath, 0);
	}
	return texture;
}


//将Assimp的aiMatrix转化为openGL的glmat4
glm::mat4 AssimpLoader::getMat4f(aiMatrix4x4 value) {
	glm::mat4 to(
		value.a1, value.a2, value.a3, value.a4,
		value.b1, value.b2, value.b3, value.b4,
		value.c1, value.c2, value.c3, value.c4,
		value.d1, value.d2, value.d3, value.d4
	);

	return to;
}