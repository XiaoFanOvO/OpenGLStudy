#pragma once
#include "shadow.h"

class DirectionalLightCSMShadow : public Shadow
{
public:
	DirectionalLightCSMShadow();
	~DirectionalLightCSMShadow();

	void generateCascadeLayers(std::vector<float>& layers, float near, float far);

	void setRenderTargetSize(int width, int height) override;

	int mLayerCount = 5;
};

