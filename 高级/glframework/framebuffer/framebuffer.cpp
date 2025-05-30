#include "framebuffer.h"

Framebuffer* Framebuffer::createShadowFbo(unsigned int width, unsigned int height) {
	Framebuffer* fb = new Framebuffer();
	fb->mWidth = width;
	fb->mHeight = height;

	glGenFramebuffers(1, &fb->mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);

	fb->mDepthAttachment = Texture::createDepthAttachment(width, height, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb->mDepthAttachment->getTexture(), 0);
	
	glDrawBuffer(GL_NONE);//显式的告诉opengl， 我们当前这个fbo没有颜色输出
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fb;
}

Framebuffer* Framebuffer::createCSMShadowFbo(unsigned int width, unsigned int height, unsigned int layerNumber) {
	Framebuffer* fb = new Framebuffer();

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//加入深度附件
	Texture* depthAttachment = Texture::createDepthAttachmentCSMArray(width, height, layerNumber, 0);
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthAttachment->getTexture(), 0, 0);//暂时绑定到第零层的layer
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	fb->mFBO = fbo;
	fb->mDepthAttachment = depthAttachment;
	fb->mWidth = width;
	fb->mHeight = height;

	return fb;
}

Framebuffer* Framebuffer::createPointShadowFBO(unsigned int width, unsigned int height) {
	Framebuffer* fb = new Framebuffer();

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	Texture* depthAttachment = Texture::createDepthAttachmentCubeMap(width, height, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X, depthAttachment->getTexture(), 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	fb->mFBO = fbo;
	fb->mDepthAttachment = depthAttachment;
	fb->mWidth = width;
	fb->mHeight = height;

	return fb;
}

Framebuffer* Framebuffer::createMultiSampleFbo(unsigned int width, unsigned int height, unsigned int samples) {
	Framebuffer* fb = new Framebuffer();

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	auto colorAttachment = Texture::createMultiSampleTexture(width, height, samples, GL_RGBA, 0);
	auto dsAttachment = Texture::createMultiSampleTexture(width, height, samples, GL_DEPTH24_STENCIL8, 0);

	//绑定texture到FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorAttachment->getTexture(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, dsAttachment->getTexture(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	fb->mFBO = fbo;
	fb->mDepthStencilAttachment = dsAttachment;
	fb->mColorAttachment = colorAttachment;
	fb->mWidth = width;
	fb->mHeight = height;

	return fb;
}


Framebuffer* Framebuffer::createHDRFbo(unsigned int width, unsigned int height) {
	Framebuffer* fb = new Framebuffer();

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	auto colorAttachment = Texture::createHDRTexture(width, height, 0);
	auto dsAttachment = Texture::createDepthStencilAttachment(width, height, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment->getTexture(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, dsAttachment->getTexture(), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	fb->mFBO = fbo;
	fb->mDepthStencilAttachment = dsAttachment;
	fb->mColorAttachment = colorAttachment;
	fb->mWidth = width;
	fb->mHeight = height;

	return fb;
}

Framebuffer* Framebuffer::createHDRBloomFbo(unsigned int width, unsigned int height) {
	Framebuffer* fb = new Framebuffer();

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	auto colorAttachment = Texture::createHDRTexture(width, height, 0);
	//auto dsAttachment = Texture::createDepthStencilAttachment(width, height, 0);// bloom只需要颜色

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment->getTexture(), 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, dsAttachment->getTexture(), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	fb->mFBO = fbo;
	//fb->mDepthStencilAttachment = dsAttachment;
	fb->mColorAttachment = colorAttachment;
	fb->mWidth = width;
	fb->mHeight = height;

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