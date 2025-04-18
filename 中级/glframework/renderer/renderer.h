#pragma once
#include <vector>
#include <iostream>
#include "../core.h"
#include "../mesh/mesh.h"
#include "../../application/camera/camera.h"
#include "../light/directionalLight.h"
#include "../light/ambientLight.h"
#include "../light/pointLight.h"
#include "../light/spotLight.h"
#include "../shader.h"
#include "../material/phongMaterial.h"
#include "../material/whiteMaterial.h"
#include "../material/opacityMaskMaterial.h"
#include "../material/screenMaterial.h"
#include "../material/cubeMaterial.h"
#include "../material/phongEnvMaterial.h"
#include "../material/phongInstanceMaterial.h"
#include "../mesh/instancedMesh.h"
#include <string>
#include "../scene.h"
#include <algorithm>

class Renderer
{
public:
	Renderer();
	~Renderer();

	//渲染功能函数:
	//1 每次调用都会渲染一帧
	//void render(const std::vector<Mesh*>& meshes,
	//	Camera* camera,
	//	DirectionalLight* dirLight,
	//	const std::vector<PointLight*>& pointLights,
	//	SpotLight* spotLight,
	//	AmbientLight* ambLight
	//);

	void render(Scene* scene, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight, unsigned int fbo = 0);

	//void render(const std::vector<Mesh*>& meshes,
	//	Camera* camera,
	//	DirectionalLight* dirLight,
	//	AmbientLight* ambLight
	//);

	/*void render(const std::vector<Mesh*>& meshes,
		Camera* camera,
		PointLight* pointLight,
		AmbientLight* ambLight
	);*/

	void renderObject(Object* object, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight);

	void printMatrix(const glm::mat4& mat);

	void setClearColor(glm::vec3 color);

	Material* mGlobalMaterial{ nullptr };

private:
	void projectObject(Object* obj);

	//根据Material类型不同,挑选不同的shader
	Shader* pickShader(MaterialType type);

	void setDepthState(Material* material);
	void setPolygonOffsetState(Material* material);
	void setStencilState(Material* material);
	void setBlendState(Material* material);
	void setFaceCullingState(Material* material);

	//生成多种不同的shader对象
	//根据材质类型的不同,挑选使用哪一个shader对象
	Shader* mPhongShader{ nullptr };
	Shader* mWhiteShader{ nullptr };
	Shader* mDepthShader{ nullptr };
	Shader* mOpacityMaskShader{ nullptr };
	Shader* mScreenShader{ nullptr };
	Shader* mCubeShader{ nullptr };
	Shader* mPhongEnvShader{ nullptr };
	Shader* mPhongInstancedShader{ nullptr };

	//不透明物体与透明物体的队列
	//注意!! 每一帧绘制前需要清空两个队列
	std::vector<Mesh*> mOpacityObjects{};
	std::vector<Mesh*> mTransparentObjects{};
};

