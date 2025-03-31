#pragma once
#include "core.h"
#include <string>

class Texture
{
public:
	Texture(const std::string& path, unsigned int unit);
	~Texture();

	void bind();//将texture挂到openGL状态机上

	int getWidth()  { return mWidth; }
	int getHeight()  { return mHeight; }

private:
	GLuint mTexture{ 0 };
	int mWidth{ 0 };
	int mHeight{ 0 };
	unsigned int mUnit{ 0 };//绑定的纹理单元 GL_TEXTURE0 + mUnit
};
