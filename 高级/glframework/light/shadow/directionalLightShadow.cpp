#include "directionalLightShadow.h"
#include "../../../application/camera/orthographicCamera.h"

DirectionalLightShadow::DirectionalLightShadow() {
	float size = 10.0f;
	mCamera = new OrthographicCamera(-size, size, -size, size, 0.1f, 80.0f);
	mRenderTarget = Framebuffer::createShadowFbo(1024, 1024);
}

DirectionalLightShadow::~DirectionalLightShadow() {
	delete mCamera;
	delete mRenderTarget;
}


void DirectionalLightShadow::setRenderTargetSize(int width, int height)  {
	if (mRenderTarget != nullptr)
	{
		delete mRenderTarget;
	}
	mRenderTarget = Framebuffer::createShadowFbo(width, height);
}

glm::mat4 DirectionalLightShadow::getLightMatrix(glm::mat4 lightModelMatrix) {
	auto viewMatrix = glm::inverse(lightModelMatrix);
	auto projMatrix = mCamera->getProjectionMatrix();
	return  projMatrix * viewMatrix;
}