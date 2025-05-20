#pragma once

#include "../core.h"
#include "../texture.h"

class Framebuffer {
public:
	static Framebuffer* createShadowFbo(unsigned int width, unsigned int height);

	Framebuffer();
	Framebuffer(unsigned int width, unsigned int height);
	~Framebuffer();

public:
	unsigned int mWidth{ 0 };
	unsigned int mHeight{ 0 };

	unsigned int mFBO{ 0 };
	Texture* mColorAttachment{ nullptr };
	Texture* mDepthStencilAttachment{ nullptr };
	Texture* mDepthAttachment{ nullptr };
};