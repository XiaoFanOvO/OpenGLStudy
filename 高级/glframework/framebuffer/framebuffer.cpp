#include "framebuffer.h"


Framebuffer* Framebuffer::createShadowFbo(unsigned int width, unsigned int height) {
	Framebuffer* fb = new Framebuffer();
	fb->mWidth = width;
	fb->mHeight = height;

	glGenFramebuffers(1, &fb->mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);

	fb->mDepthAttachment = Texture::createDepthAttachment(width, height, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb->mDepthAttachment->getTexture(), 0);//最后这个0是0号mipmap,也就是贴图本身

	glDrawBuffer(GL_NONE); //显式地告诉opengl,我们当前这个fbo没有颜色输出
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//解绑
	
	return fb;
}

Framebuffer::Framebuffer() {
	
}

Framebuffer::Framebuffer(unsigned int width, unsigned int height) {
	mWidth = width;
	mHeight = height;

	//1 生成fbo对象并且绑定
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);


	//2 生成颜色附件，并且加入fbo
	mColorAttachment = Texture::createColorAttachment(mWidth, mHeight, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorAttachment->getTexture(), 0);

	//3 生成depth Stencil附件，加入fbo
	mDepthStencilAttachment = Texture::createDepthStencilAttachment(mWidth, mHeight, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthStencilAttachment->getTexture(), 0);

	//检查当前构建的fbo是否完整
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