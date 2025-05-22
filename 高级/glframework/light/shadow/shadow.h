#pragma once
#include "../../core.h"
#include "../../framebuffer/framebuffer.h"
#include "../../../application/camera/camera.h"


class Shadow
{
public:
	Shadow();
	~Shadow();

	//不能动态调整FBO 所以只能先干掉原来的 然后创建一个新的
	virtual void setRenderTargetSize(int width, int height) = 0;

	Camera* mCamera{ nullptr };
	Framebuffer* mRenderTarget{ nullptr };

	float mBias{ 0.0003f };
	float mPcfRadius{ 0.01f };
	float mDiskTightness{ 1.0f };

private:

};
