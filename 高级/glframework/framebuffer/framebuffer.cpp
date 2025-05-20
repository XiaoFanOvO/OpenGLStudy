#include "framebuffer.h"


Framebuffer* Framebuffer::createShadowFbo(unsigned int width, unsigned int height) {
	Framebuffer* fb = new Framebuffer();
	fb->mWidth = width;
	fb->mHeight = height;

	glGenFramebuffers(1, &fb->mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);

	fb->mDepthAttachment = Texture::createDepthAttachment(width, height, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb->mDepthAttachment->getTexture(), 0);//������0��0��mipmap,Ҳ������ͼ����

	glDrawBuffer(GL_NONE); //��ʽ�ظ���opengl,���ǵ�ǰ���fboû����ɫ���
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//���
	
	return fb;
}

Framebuffer::Framebuffer() {
	
}

Framebuffer::Framebuffer(unsigned int width, unsigned int height) {
	mWidth = width;
	mHeight = height;

	//1 ����fbo�����Ұ�
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);


	//2 ������ɫ���������Ҽ���fbo
	mColorAttachment = Texture::createColorAttachment(mWidth, mHeight, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorAttachment->getTexture(), 0);

	//3 ����depth Stencil����������fbo
	mDepthStencilAttachment = Texture::createDepthStencilAttachment(mWidth, mHeight, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilAttachment->getTexture(), 0);

	//��鵱ǰ������fbo�Ƿ�����
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error:FrameBuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
	if (mFBO) {
		glDeleteFramebuffers(1, &mFBO);
	}
	if (mColorAttachment != nullptr) {
		delete mColorAttachment;
	}

	if (mDepthStencilAttachment != nullptr) {
		delete mDepthStencilAttachment;
	}
}