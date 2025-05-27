#include "directionalLightCSMShadow.h"
#include "../../../application/camera/orthographicCamera.h"
#include "../../../application/camera/perspectiveCamera.h"
#include "../../tools/tools.h"


DirectionalLightCSMShadow::DirectionalLightCSMShadow() {
	mRenderTarget = Framebuffer::createCSMShadowFbo(1024, 1024, mLayerCount);
}

DirectionalLightCSMShadow::~DirectionalLightCSMShadow() {
	delete mRenderTarget;
}


void DirectionalLightCSMShadow::setRenderTargetSize(int width, int height)  {
	if (mRenderTarget != nullptr)
	{
		delete mRenderTarget;
	}
	mRenderTarget = Framebuffer::createCSMShadowFbo(width, height, mLayerCount);
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
	//1 求取当前子视椎体的八个角点世界坐标系的值(传入的是视角相机)
	auto perpCamera = (PerspectiveCamera*)camera;
	auto perpViewMatrix = perpCamera->getViewMatrix();
	auto perpProjectionMatrix = glm::perspective(glm::radians(perpCamera->mFovy), perpCamera->mAspect, near, far);
	auto corners = Tools::getFrustumCornersWorldSpace(perpProjectionMatrix * perpViewMatrix);

	//2 八个角点位置的平均值作为光源的位置,得到lightViewMatrix
	glm::vec3 center = glm::vec3(0.0f);
	for (int i = 0; i < corners.size(); i++)
	{
		center += glm::vec3(corners[i]);
	}
	center /= corners.size();

	auto lightViewMatrix = glm::lookAt(center, center + lightDir, glm::vec3(0.0, 1.0, 0.0));

	//3 将八个角点转化到光源坐标系,并求最小的AABB包围盒(把包围盒作为投影矩形)
	float minX = std::numeric_limits<float>::max();//float数据的最大值(避免超框)
	float maxX = std::numeric_limits<float>::min();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::min();

	for (const auto& v : corners )
	{	
		//将v点从世界坐标系下转换到了光源相机坐标系下
		const auto pt = lightViewMatrix * v;
		minX = std::min(minX, pt.x);
		maxX = std::max(maxX, pt.x);
		minY = std::min(minY, pt.y);
		maxY = std::max(maxY, pt.y);
		minZ = std::min(minZ, pt.z);
		maxZ = std::max(maxZ, pt.z);
	}
	//4 调整(包围盒以外的物体,也能够影响到其内部物体的阴影遮挡效果)
	maxZ *= 10;
	minZ *= 10;

	//5 计算当前光源的投影矩阵
	//这里近平面和远平面把光源夹在中间  ortho中near和far又表示的是距离值,离远点的距离
	//所以近平面在Z轴正方向,原点左边,和原点的距离是负数,远平面在Z轴负方向,原点右边,和原点的距离是正值
	//所以这里都需要做相反数处理
	//比如近平面是10,原点是0,远平面是-10,那么近平面离远点的距离就是0-10 远平面离远点的距离就是0-(-10)
	auto lightProjectionMatrix = glm::ortho(minX, maxX, minY, maxY, -maxZ, -minZ);

	return lightProjectionMatrix * lightViewMatrix;
}

//计算每个视椎体的LightMatrix
std::vector<glm::mat4> DirectionalLightCSMShadow::getLightMatrices(Camera* camera, glm::vec3 lightDir, const std::vector<float>& clips) {
	std::vector<glm::mat4> matrices;
	for (int i = 0; i < clips.size() - 1; i++)
	{
		auto lightMatrix = getLightMatrix(camera, lightDir, clips[i], clips[i + 1]);
		matrices.push_back(lightMatrix);
	}
	return matrices;
}