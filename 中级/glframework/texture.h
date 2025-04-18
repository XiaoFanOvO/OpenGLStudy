#pragma once
#include"core.h"
#include <string>

class Texture {
public:
	static Texture* createTexture(const std::string& path, unsigned int unit);
	static Texture* createTextureFromMemory(
		const std::string& path, 
		unsigned int unit,
		unsigned char* dataIn,	//内存指针
		uint32_t widthIn,		//宽度
		uint32_t heightIn		//高度
	);
	static Texture* createColorAttachment(unsigned int width, unsigned int height, unsigned int unit);
	static Texture* createDepthStencilAttachment(unsigned int width, unsigned int height, unsigned int unit);

	Texture();
	Texture(const std::string& path, unsigned int unit); //从硬盘上直接读取图片进行初始化
	Texture(unsigned int unit,		//挂载的纹理单元
			unsigned char* dataIn,	//内存指针
			uint32_t widthIn,		//宽度
			uint32_t heightIn		//高度
		);//从内存中读取图片并初始化(部分fbx加载模型的时候会把贴图也一起加载到内存中)
	Texture(unsigned int width, unsigned int height, unsigned int unit);
	Texture(const std::vector<std::string>& paths, unsigned int unit);
	~Texture();

	void bind();
	void setUnit(unsigned int unit) { mUnit = unit; }
	 
	int getWidth()const { return mWidth; }
	int getHeight()const { return mHeight; }
	GLuint getTexture()const { return mTexture; }

private:
	GLuint mTexture{ 0 };
	int mWidth{ 0 };
	int mHeight{ 0 };
	unsigned int mUnit{ 0 };
	unsigned int mTextureTarget{ GL_TEXTURE_2D};


	//注意: 静态!! 属于类的 不属于某个对象 
	static std::map<std::string, Texture*> mTextureCache;
};