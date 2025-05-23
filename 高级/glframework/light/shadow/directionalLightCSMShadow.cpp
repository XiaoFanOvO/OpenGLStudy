#include "directionalLightCSMShadow.h"
#include "../../../application/camera/orthographicCamera.h"

DirectionalLightCSMShadow::DirectionalLightCSMShadow() {
	
}

DirectionalLightCSMShadow::~DirectionalLightCSMShadow() {
	
}


void DirectionalLightCSMShadow::setRenderTargetSize(int width, int height)  {
	
}

//���ɷֲ�����ݼ�
void DirectionalLightCSMShadow::generateCascadeLayers(std::vector<float>& layers, float near, float far) {
	layers.clear();

	for (int i = 0; i <= mLayerCount; i++) //ѭ��layerCount+1��,�պúͰ���������
	{
		float layer = near * glm::pow((far / near), (float)i /(float) mLayerCount);
		layers.push_back(layer);
	}
}