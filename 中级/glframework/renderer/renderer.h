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

	//��Ⱦ���ܺ���:
	//1 ÿ�ε��ö�����Ⱦһ֡
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

	//����Material���Ͳ�ͬ,��ѡ��ͬ��shader
	Shader* pickShader(MaterialType type);

	void setDepthState(Material* material);
	void setPolygonOffsetState(Material* material);
	void setStencilState(Material* material);
	void setBlendState(Material* material);
	void setFaceCullingState(Material* material);

	//���ɶ��ֲ�ͬ��shader����
	//���ݲ������͵Ĳ�ͬ,��ѡʹ����һ��shader����
	Shader* mPhongShader{ nullptr };
	Shader* mWhiteShader{ nullptr };
	Shader* mDepthShader{ nullptr };
	Shader* mOpacityMaskShader{ nullptr };
	Shader* mScreenShader{ nullptr };
	Shader* mCubeShader{ nullptr };
	Shader* mPhongEnvShader{ nullptr };
	Shader* mPhongInstancedShader{ nullptr };

	//��͸��������͸������Ķ���
	//ע��!! ÿһ֡����ǰ��Ҫ�����������
	std::vector<Mesh*> mOpacityObjects{};
	std::vector<Mesh*> mTransparentObjects{};
};

