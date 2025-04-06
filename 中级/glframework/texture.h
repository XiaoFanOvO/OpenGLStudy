#pragma once
#include"core.h"
#include <string>

class Texture {
public:
	static Texture* createTexture(const std::string& path, unsigned int unit);
	static Texture* createTextureFromMemory(
		const std::string& path, 
		unsigned int unit,
		unsigned char* dataIn,	//�ڴ�ָ��
		uint32_t widthIn,		//���
		uint32_t heightIn		//�߶�
	);


	Texture(const std::string& path, unsigned int unit); //��Ӳ����ֱ�Ӷ�ȡͼƬ���г�ʼ��
	Texture(unsigned int unit,		//���ص�����Ԫ
			unsigned char* dataIn,	//�ڴ�ָ��
			uint32_t widthIn,		//���
			uint32_t heightIn		//�߶�
		);//���ڴ��ж�ȡͼƬ����ʼ��(����fbx����ģ�͵�ʱ������ͼҲһ����ص��ڴ���)
	~Texture();

	void bind();
	void setUnit(unsigned int unit) { mUnit = unit; }

	int getWidth()const { return mWidth; }
	int getHeight()const { return mHeight; }

private:
	GLuint mTexture{ 0 };
	int mWidth{ 0 };
	int mHeight{ 0 };
	unsigned int mUnit{ 0 };


	//ע��: ��̬!! ������� ������ĳ������ 
	static std::map<std::string, Texture*> mTextureCache;
};