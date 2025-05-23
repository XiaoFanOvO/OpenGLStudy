#pragma once
#include <vector>
#include "../core.h"
#include "../mesh/mesh.h"
#include "../../application/camera/camera.h"
#include "../light/directionalLight.h"
#include "../light/pointLight.h"
#include "../light/spotLight.h"
#include "../light/ambientLight.h"
#include "../shader.h"
#include "../scene.h"
#include "../framebuffer/framebuffer.h"

class Renderer {
public:
	Renderer();
	~Renderer();

	//��Ⱦ���ܺ�����
	//	ÿ�ε��ö�����Ⱦһ֡
	void render(
		Scene* scene,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight,
		unsigned int fbo = 0
	);

	void renderObject(
		Object* object,
		Camera* camera,
		DirectionalLight* dirLight,
		AmbientLight* ambLight
	);

	void renderShadowMap(const std::vector<Mesh*>& meshes, DirectionalLight* dirLight, Framebuffer* fbo);


	//void render(
	//	const std::vector<Mesh*>& meshes,
	//	Camera* camera,
	//	PointLight* pointLight,
	//	AmbientLight* ambLight
	//);

	//void render(
	//	const std::vector<Mesh*>& meshes,
	//	Camera* camera,
	//	DirectionalLight* dirLight,
	//	AmbientLight* ambLight
	//);

	void setClearColor(glm::vec3 color);

public:
	Material* mGlobalMaterial{ nullptr };

private:
	void projectObject(Object* obj);

	//����Material���Ͳ�ͬ����ѡ��ͬ��shader
	Shader* pickShader(MaterialType type);

	void setDepthState(Material* material);
	void setPolygonOffsetState(Material* material);
	void setStencilState(Material* material);
	void setBlendState(Material* material);
	void setFaceCullingState(Material* material);

private:
	//���ɶ��ֲ�ͬ��shader����
	//���ݲ������͵Ĳ�ͬ����ѡʹ����һ��shader����
	Shader* mPhongShader{ nullptr };
	Shader* mWhiteShader{ nullptr };
	Shader* mDepthShader{ nullptr };
	Shader* mOpacityMaskShader{ nullptr };
	Shader* mScreenShader{ nullptr };
	Shader* mCubeShader{ nullptr };
	Shader* mPhongEnvShader{ nullptr };
	Shader* mPhongInstanceShader{ nullptr };
	Shader* mGrassInstanceShader{ nullptr };

	Shader* mPhongNormalShader{ nullptr };
	Shader* mPhongParallaxShader{ nullptr };

	Shader* mShadowShader{ nullptr };
	Shader* mPhongShadowShader{ nullptr };
	Shader* mPhongCSMShadowShader{ nullptr };

	//��͸��������͸������Ķ���
	//ע�⣡��*****ÿһ֡����ǰ����Ҫ�����������********
	std::vector<Mesh*>	mOpacityObjects{};
	std::vector<Mesh*>	mTransparentObjects{};

};