#include "directionalLightCSMShadow.h"
#include "../../../application/camera/orthographicCamera.h"

DirectionalLightCSMShadow::DirectionalLightCSMShadow() {
	
}

DirectionalLightCSMShadow::~DirectionalLightCSMShadow() {
	
}


void DirectionalLightCSMShadow::setRenderTargetSize(int width, int height)  {
	
}

//生成分层的数据集
void DirectionalLightCSMShadow::generateCascadeLayers(std::vector<float>& layers, float near, float far) {
	layers.clear();

	for (int i = 0; i <= mLayerCount; i++) //循环layerCount+1次,刚好和板子数对上
	{
		float layer = near * glm::pow((far / near), (float)i /(float) mLayerCount);
		layers.push_back(layer);
	}
}