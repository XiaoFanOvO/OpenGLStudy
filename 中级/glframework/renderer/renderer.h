#pragma once
#include <vector>
#include <iostream>
#include "../core.h"
#include "../mesh.h"
#include "../../application/camera/camera.h"
#include "../light/directionalLight.h"
#include "../light/ambientLight.h"
#include "../light/pointLight.h"
#include "../light/spotLight.h"
#include "../shader.h"
#include "../material/phongMaterial.h"
#include "../material/whiteMaterial.h"
#include <string>
#include "../scene.h"

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

	void render(Scene* scene, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight);

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

private:
	//����Material���Ͳ�ͬ,��ѡ��ͬ��shader
	Shader* pickShader(MaterialType type);

	//���ɶ��ֲ�ͬ��shader����
	//���ݲ������͵Ĳ�ͬ,��ѡʹ����һ��shader����
	Shader* mPhongShader{ nullptr };
	Shader* mWhiteShader{ nullptr };
	Shader* mDepthShader{ nullptr };
};

