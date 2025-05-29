#include "pointLightShadow.h"
#include "../../../application/camera/perspectiveCamera.h"

PointLightShadow::PointLightShadow() {
	mCamera = new PerspectiveCamera(90.0f, 1.0f, 0.1f, 50.0f);
	mRenderTarget = Framebuffer::createPointShadowFBO(1024, 1024);
}

PointLightShadow::~PointLightShadow() {
	delete mCamera;
	delete mRenderTarget;
}


void PointLightShadow::setRenderTargetSize(int width, int height)  {
	if (mRenderTarget != nullptr)
	{
		delete mRenderTarget;
	}mRenderTarget = Framebuffer::createPointShadowFBO(width, height);
}


std::vector<glm::mat4> PointLightShadow::getLightMatrices(glm::vec3 lightPos) {
	auto shadowProj = mCamera->getProjectionMatrix();

	std::vector<glm::mat4> lightMatrices;
	lightMatrices.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0))); // +X
	lightMatrices.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0))); // -X
	lightMatrices.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));  // +Y
	lightMatrices.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0))); // -Y
	lightMatrices.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));  // +Z
	lightMatrices.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))); // -Z

	return lightMatrices;
}