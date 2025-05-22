#pragma once
#include "shadow.h"

class DirectionalLightShadow : public Shadow
{
public:
	DirectionalLightShadow();
	~DirectionalLightShadow();

	void setRenderTargetSize(int width, int height) override;

	glm::mat4 getLightMatrix(glm::mat4 lightModelMatrix);

private:

};

