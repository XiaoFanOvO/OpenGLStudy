#pragma once
#include "shadow.h"

class DirectionalLightCSMShadow : public Shadow
{
public:
	DirectionalLightCSMShadow();
	~DirectionalLightCSMShadow();

	void generateCascadeLayers(std::vector<float>& layers, float near, float far);

	void setRenderTargetSize(int width, int height) override;

	//计算当前光源方向下,子椎体的LightMatrix
	glm::mat4 getLightMatrix(Camera* camera, glm::vec3 lightDir, float near, float far);
	std::vector<glm::mat4> getLightMatrices(Camera* camera, glm::vec3 lightDir, const std::vector<float>& clips);

	int mLayerCount = 7;
};

