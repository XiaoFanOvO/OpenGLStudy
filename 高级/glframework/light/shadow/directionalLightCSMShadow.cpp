#include "directionalLightCSMShadow.h"
#include "../../../application/camera/orthographicCamera.h"
#include "../../../application/camera/perspectiveCamera.h"

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

//计算当前光源方向下,子椎体的LightMatrix
//这个camera一定是透视投影相机(才有视椎体这个概念)
glm::mat4 DirectionalLightCSMShadow::getLightMatrix(Camera* camera, glm::vec3 lightDir, float near, float far) {
	//1 求取当前子视椎体的八个角点世界坐标系的值


	//2 八个角点位置的平均值作为光源的位置,得到lightViewMatrix


	//3 将八个角点转化到光源坐标系,并求最小的AABB包围盒

	//4 调整
}