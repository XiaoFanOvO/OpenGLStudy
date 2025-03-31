#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION 
#include "../application/stb_image.h"


Texture::Texture(const std::string& path, unsigned int unit) {
	//����ͼƬ���Դ沢����Ҫ�󶨵�����Ԫ��
	mUnit = unit;
	//1 stbImage ��ȡͼƬ
	int channels;
	//--��תy��
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &mWidth, &mHeight, &channels, STBI_rgb_alpha);//��ȡ����ȫ��ת��ΪRGBA��ʽ
	//2 ���������Ҽ��Ԫ��
	glGenTextures(1, &mTexture);
	//--��������Ԫ--
	glActiveTexture(GL_TEXTURE0 + mUnit);
	//--���������--
	glBindTexture(GL_TEXTURE_2D, mTexture);//������Զ������漤���0������Ԫ����
	//3 ������������ �����Դ� ��CPU��GPU
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);//opengl�Զ�����mipmap
	
	////����ֶ�ʵ��mipmap����(��Ҫ�޸�filter)
	//int width = mWidth, height = mHeight;
	////����ÿ��mipmap�㼶,Ϊÿ�������mipmap���ͼƬ����
	//for (int level = 0; true; ++level)
	//{
	//	//1 ����ǰ�����mipmap��Ӧ�����ݷ���gpu��
	//	glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//	//2 �ж��Ƿ��˳�ѭ��
	//	if (width == 1 && height == 1)
	//	{
	//		break;
	//	}
	//	//3 ������һ��ѭ���Ŀ��/�߶� ����2
	//	width = width > 1 ? width / 2 : 1;
	//	height = height > 1 ? height / 2 : 1;
	//}

	//***�ͷ�����
	stbi_image_free(data);

	//4 ����������˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // ��Ҫ������>ͼƬ���� ��˫���Բ�ֵ
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);// ��Ҫ������<ͼƬ���� ���ٽ�����
	//��Ӱ�쵽�ֶ�����mipmap��ʵ��,��Ҫ��Ϊ����Ĵ���
	//*********��Ҫ************
	//GL_NEAREST:�ڵ���mipmap����ʱ��NEARESTҲ�����ٽ�����    
	// GL_LINEAR
	// 
	//MIPMAP_LINEAR:������mipmap֮��������Բ�ֵ
	//MIPMAP_NEAREST
	//�����������������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);//�ص�֮������shader�ڲ��Ĳ�����ʽȥ��

	//5 ��������İ�����ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//v

	//����BUG ���������ô���õĻ�����ʾ����
	//�Ѿ����ʱ������Ԫ���Ǽ���״̬,��ֱ�����õ�ǰ����Ϊ0,�൱�������֮ǰ���õ�״̬
	//����Ԫ����֮����û���ص���! ֻ�м�����������Ԫ
	//glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::~Texture() {
	if (mTexture != 0)
	{
		glDeleteTextures(1, &mTexture);
	}
}
 
void Texture::bind() {
	//���л�����Ԫ,Ȼ���texture����
	glActiveTexture(GL_TEXTURE0 + mUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}
