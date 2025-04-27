#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../application/stb_image.h"

std::map<std::string, Texture*> Texture::mTextureCache{};

Texture* Texture::createTexture(const std::string& path, unsigned int unit) {
	//1 ����Ƿ񻺴����·����Ӧ���������
	auto iter = mTextureCache.find(path);
	if (iter != mTextureCache.end()) {
		//����iterater������������first-��key  second-��value
		return iter->second;
	}

	//2 �����·����Ӧ��textureû�����ɹ�������������
	auto texture = new Texture(path, unit, GL_SRGB_ALPHA);
	mTextureCache[path] = texture;

	return texture;
}

Texture* Texture::createTextureFromMemory(
	const std::string& path,
	unsigned int unit,
	unsigned char* dataIn,
	uint32_t widthIn,
	uint32_t heightIn
) {
	//1 ����Ƿ񻺴����·����Ӧ���������
	auto iter = mTextureCache.find(path);
	if (iter != mTextureCache.end()) {
		//����iterater������������first-��key  second-��value
		return iter->second;
	}

	//2 �����·����Ӧ��textureû�����ɹ�������������
	auto texture = new Texture(unit, dataIn, widthIn, heightIn, GL_SRGB_ALPHA);
	mTextureCache[path] = texture;

	return texture;
}

Texture* Texture::createColorAttachment(
	unsigned int width,
	unsigned int height,
	unsigned int unit
) {
	return new Texture(width, height, unit);
}

Texture* Texture::createDepthStencilAttachment(
	unsigned int width,
	unsigned int height,
	unsigned int unit
) {
	Texture* dsTex = new Texture();

	unsigned int depthStencil;
	glGenTextures(1, &depthStencil);
	glBindTexture(GL_TEXTURE_2D, depthStencil);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	dsTex->mTexture = depthStencil;
	dsTex->mWidth = width;
	dsTex->mHeight = height;
	dsTex->mUnit = unit;

	return dsTex;
}

Texture::Texture() {

}
Texture::Texture(const std::string& path, unsigned int unit, unsigned int internalFormat) {
	mUnit = unit;

	//1 stbImage ��ȡͼƬ
	int channels;

	//--��תy��
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(path.c_str(), &mWidth, &mHeight, &channels, STBI_rgb_alpha);

	//2 ���������Ҽ��Ԫ��
	glGenTextures(1, &mTexture);
	//--��������Ԫ--
	glActiveTexture(GL_TEXTURE0 + mUnit);
	//--���������--	
	glBindTexture(GL_TEXTURE_2D, mTexture);

	//3 ������������,�����Դ�
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	
//	glGenerateMipmap(GL_TEXTURE_2D);

	//***�ͷ����� 
	stbi_image_free(data);

	//4 ��������Ĺ��˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	//5 ��������İ�����ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//v
}

Texture::Texture(
	unsigned int unit,
	unsigned char* dataIn,
	uint32_t widthIn,
	uint32_t heightIn,
	unsigned int internalFormat
) {
	mUnit = unit;

	//1 stbImage ��ȡͼƬ
	int channels;

	//--��תy��
	stbi_set_flip_vertically_on_load(true);

	//��������ͼƬ�Ĵ�С
	//Assimp�涨�������Ƕ������png����jpgѹ����ʽ�Ļ���height = 0��width�ʹ�����ͼƬ��С
	uint32_t dataInSize = 0;
	if (!heightIn) {
		dataInSize = widthIn;
	}
	else {
		//�����ǶͼƬ����ѹ����ʽ��height = �����߶ȣ�width = �������
		//͵����ͳһ��Ϊ���ݸ�ʽ����RGBA
		dataInSize = widthIn * heightIn * 4;//�ֽ�Ϊ��λ
	}


	unsigned char* data = stbi_load_from_memory(dataIn, dataInSize, &mWidth, &mHeight, &channels, STBI_rgb_alpha);

	//2 ���������Ҽ��Ԫ��
	glGenTextures(1, &mTexture);
	//--��������Ԫ--
	glActiveTexture(GL_TEXTURE0 + mUnit);
	//--���������--
	glBindTexture(GL_TEXTURE_2D, mTexture);

	//3 ������������,�����Դ�
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	//***�ͷ����� 
	stbi_image_free(data);

	//4 ��������Ĺ��˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	//5 ��������İ�����ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//v
}

Texture::Texture(unsigned int width, unsigned int height, unsigned int unit, unsigned int internalFormat) {
	mWidth = width;
	mHeight = height;
	mUnit = unit;
	
	glGenTextures(1, &mTexture);
	glActiveTexture(GL_TEXTURE0 + mUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

//paths:�������º�ǰ(+x -x +y -y +z -z)
Texture::Texture(const std::vector<std::string>& paths, unsigned int unit, unsigned int internalFormat) {
	mUnit = unit;
	mTextureTarget = GL_TEXTURE_CUBE_MAP;

	//cubemap����Ҫ��תy��
	stbi_set_flip_vertically_on_load(false);

	//1 ����CubeMap����
	glGenTextures(1, &mTexture);
	glActiveTexture(GL_TEXTURE0 + mUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);


	//2 ѭ����ȡ������ͼ�����ҷ��õ�cubemap������GPU�ռ���
	int channels;
	int width = 0, height = 0;
	unsigned char* data = nullptr;
	for (int i = 0; i < paths.size(); i++) {
		data = stbi_load(paths[i].c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (data != nullptr) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Error: CubeMap Texture failed to load at path����" << paths[i] << std::endl;
			stbi_image_free(data);
		}
	}

	//3 �����������
	glTexParameteri(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);//u
	glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);//v
}


Texture::~Texture() {
	if (mTexture != 0) {
		glDeleteTextures(1, &mTexture);
	}
}

void Texture::bind() {
	//���л�����Ԫ��Ȼ���texture����
	glActiveTexture(GL_TEXTURE0 + mUnit);
	glBindTexture(mTextureTarget, mTexture);
}