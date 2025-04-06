#include "renderer.h"

Renderer::Renderer()
{
	mPhongShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	mWhiteShader = new Shader("assets/shaders/white.vert", "assets/shaders/white.frag");
}

Renderer::~Renderer()
{
}


void Renderer::setClearColor(glm::vec3 color) {
	glClearColor(color.r, color.g, color.b, 1.0);
}


void Renderer::render(Scene* scene, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight) {
	//1 设置当前帧绘制的时候,opengl的必要状态机参数
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//2 清理画布
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//3 将scene当做根节点 开始递归渲染
	renderObject(scene, camera, dirLight, ambLight);
}

//针对单个object进行渲染
void Renderer::renderObject(Object* object, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight) {
	//1 判断是Mesh还是Object 如果是Mesh才需要渲染
	if (object-> getType() == ObjectType::Mesh)
	{
		//具体渲染流程
		auto mesh = (Mesh*)object;
		auto geometry = mesh->mGeometry;
		auto material = mesh->mMaterial;
		//1 决定使用哪个shader
		Shader* shader = pickShader(material->mType);
		//2 更新shader的uniform
		shader->begin();
		switch (material->mType)
		{
		case MaterialType::PhongMaterial: {
			PhongMaterial* phongMat = (PhongMaterial*)material;
			//将纹理采样器与纹理单元挂钩
			//diffuse蒙版的帧更新
			shader->setInt("sampler", 0);
			//将纹理与纹理单元进行挂钩
			phongMat->mDiffuse->bind();
			//高光蒙版的帧更新
			//shader->setInt("specularMaskSampler", 1);
			//phongMat->mSpecularMask->bind();
			//MVP
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//光源参数的uniform更新 
			//directionalLight的更新
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			
			shader->setFloat("shiness", phongMat->mShiness);
			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);
		}
			break;
		case MaterialType::WhiteMaterial:
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
			break;
		default:
			break;
		}
		//3 绑定vao
		glBindVertexArray(geometry->getVao());
		//4 执行绘制命令
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
	}

	//2 遍历当前Object的子节点 对每个子节点都需要调用RenderObject(递归)
	auto children = object->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		renderObject(children[i], camera, dirLight, ambLight);
	}

}

//void Renderer::render(const std::vector<Mesh*>& meshes, Camera* camera, DirectionalLight* dirLight, const std::vector<PointLight*>& pointLights, SpotLight* spotLight, AmbientLight* ambLight) {
//	//1 设置当前帧绘制的时候,opengl的必要状态机参数
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	//2 清理画布
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	//3 遍历mesh进行绘制
//	for (int i = 0; i < meshes.size(); i++)
//	{
//		auto mesh = meshes[i];
//		auto geometry = mesh->mGeometry;
//		auto material = mesh->mMaterial;
//		//1 决定使用哪个shader
//		Shader* shader = pickShader(material->mType);
//		//2 更新shader的uniform
//		shader->begin();
//		switch (material->mType)
//		{
//		case MaterialType::PhongMaterial: {
//			PhongMaterial* phongMat = (PhongMaterial*)material;
//			//将纹理采样器与纹理单元挂钩
//			//diffuse蒙版的帧更新
//			shader->setInt("sampler", 0);
//			//将纹理与纹理单元进行挂钩
//			phongMat->mDiffuse->bind();
//			//高光蒙版的帧更新
//			shader->setInt("specularMaskSampler", 1);
//			phongMat->mSpecularMask->bind();
//			//MVP
//			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
//			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
//			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
//
//			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
//			shader->setMatrix3x3("normalMatrix", normalMatrix);
//
//			//光源参数的uniform更新 
//			//spotLight的更新
//			shader->setVector3("spotLight.position", spotLight->getPosition());
//			shader->setVector3("spotLight.color", spotLight->mColor);
//			shader->setFloat("spotLight.specularIntensity", spotLight->mSpecularIntensity);
//			shader->setVector3("spotLight.targetDirection", spotLight->mTargetDirection);
//			//shader->setFloat("visibleAngle", glm::radians(spotLight->mVisibleAngle));//注意转化为弧度 
//			shader->setFloat("spotLight.innerLine", glm::cos(glm::radians(spotLight->mInnerAngle)));
//			shader->setFloat("spotLight.outerLine", glm::cos(glm::radians(spotLight->mOuterAngle)));
//			//directionalLight的更新
//			shader->setVector3("directionalLight.color", dirLight->mColor);
//			shader->setVector3("directionalLight.direction", dirLight->mDirection);
//			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
//			//pointLights的更新
//			for (int i = 0; i < pointLights.size(); i++)
//			{
//				auto pointLight = pointLights[i];
//				std::string baseName = "pointLights[";
//				baseName.append(std::to_string(i));
//				baseName.append("]");
//				shader->setVector3(baseName + ".color", pointLight->mColor);
//				shader->setVector3(baseName + ".position", pointLight->getPosition());
//				shader->setFloat(baseName + ".specularIntensity", pointLight->mSpecularIntensity);
//				shader->setFloat(baseName + ".k2", pointLight->mK2);
//				shader->setFloat(baseName + ".k1", pointLight->mK1);
//				shader->setFloat(baseName + ".kc", pointLight->mKc);
//			}
//			
//
//			shader->setFloat("shiness", phongMat->mShiness);
//			//相机信息更新
//			shader->setVector3("cameraPosition", camera->mPosition);
//		}
//			break;
//		case MaterialType::WhiteMaterial:
//			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
//			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
//			//printMatrix(camera->getViewMatrix());
//			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
//			//printMatrix(camera->getProjectionMatrix());
//			break;
//		default:
//			continue;
//		}
//		//3 绑定vao
//		glBindVertexArray(geometry->getVao());
//		//4 执行绘制命令
//		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
//	}
//}

Shader* Renderer::pickShader(MaterialType type) {
	Shader* result = nullptr;
	switch (type)
	{
	case MaterialType::PhongMaterial:
		result = mPhongShader;
		break;
	case MaterialType::WhiteMaterial:
		result = mWhiteShader;
		break;
	default:
		std::cout << "Unknown material type to pick shader" << std::endl;
		break;
	}
	return result;
}

void Renderer::printMatrix(const glm::mat4& mat) {
	const float* data = glm::value_ptr(mat);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::cout << data[i * 4 + j] << " ";
		}
		std::cout << std::endl;
	}
}


//void Renderer::render(const std::vector<Mesh*>& meshes, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight) {
//	//1 设置当前帧绘制的时候,opengl的必要状态机参数
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	//2 清理画布
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	//3 遍历mesh进行绘制
//	for (int i = 0; i < meshes.size(); i++)
//	{
//		auto mesh = meshes[i];
//		auto geometry = mesh->mGeometry;
//		auto material = mesh->mMaterial;
//		//1 决定使用哪个shader
//		Shader* shader = pickShader(material->mType);
//		//2 更新shader的uniform
//		shader->begin();
//		switch (material->mType)
//		{
//		case MaterialType::PhongMaterial: {
//			PhongMaterial* phongMat = (PhongMaterial*)material;
//			//将纹理采样器与纹理单元挂钩
//			//diffuse蒙版的帧更新
//			shader->setInt("sampler", 0);
//			//将纹理与纹理单元进行挂钩
//			phongMat->mDiffuse->bind();
//			//高光蒙版的帧更新
//			shader->setInt("specularMaskSampler", 1);
//			phongMat->mSpecularMask->bind();
//			//MVP
//			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
//			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
//			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
//
//			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
//			shader->setMatrix3x3("normalMatrix", normalMatrix);
//
//			//光源参数的uniform更新 
//			shader->setVector3("lightDirection", dirLight->mDirection);
//			shader->setVector3("lightColor", dirLight->mColor);
//			shader->setFloat("specularIntensity", dirLight->mSpecularIntensity);
//			shader->setVector3("ambientColor", ambLight->mColor);
//			shader->setFloat("shiness", phongMat->mShiness);
//
//			//相机信息更新
//			shader->setVector3("cameraPosition", camera->mPosition);
//		}
//										break;
//		case MaterialType::WhiteMaterial:
//			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
//			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
//			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
//			break;
//		default:
//			continue;
//		}
//		//3 绑定vao
//		glBindVertexArray(geometry->getVao());
//		//4 执行绘制命令
//		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
//	}
//}


//void Renderer::render(const std::vector<Mesh*>& meshes, Camera* camera, PointLight* pointLight, AmbientLight* ambLight) {
//	//1 设置当前帧绘制的时候,opengl的必要状态机参数
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	//2 清理画布
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	//3 遍历mesh进行绘制
//	for (int i = 0; i < meshes.size(); i++)
//	{
//		auto mesh = meshes[i];
//		auto geometry = mesh->mGeometry;
//		auto material = mesh->mMaterial;
//		//1 决定使用哪个shader
//		Shader* shader = pickShader(material->mType);
//		//2 更新shader的uniform
//		shader->begin();
//		switch (material->mType)
//		{
//		case MaterialType::PhongMaterial: {
//			PhongMaterial* phongMat = (PhongMaterial*)material;
//			//将纹理采样器与纹理单元挂钩
//			//diffuse蒙版的帧更新
//			shader->setInt("sampler", 0);
//			//将纹理与纹理单元进行挂钩
//			phongMat->mDiffuse->bind();
//			//高光蒙版的帧更新
//			shader->setInt("specularMaskSampler", 1);
//			phongMat->mSpecularMask->bind();
//			//MVP
//			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
//			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
//			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
//
//			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
//			shader->setMatrix3x3("normalMatrix", normalMatrix);
//
//			//光源参数的uniform更新 
//			shader->setVector3("lightPosition", pointLight->getPosition());
//			shader->setVector3("lightColor", pointLight->mColor);
//			shader->setFloat("specularIntensity", pointLight->mSpecularIntensity);
//			shader->setVector3("ambientColor", ambLight->mColor);
//			shader->setFloat("shiness", phongMat->mShiness);
//			shader->setFloat("k2", pointLight->mK2);
//			shader->setFloat("k1", pointLight->mK1);
//			shader->setFloat("kc", pointLight->mKc);
//
//			//相机信息更新
//			shader->setVector3("cameraPosition", camera->mPosition);
//		}
//										break;
//		case MaterialType::WhiteMaterial:
//			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
//			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
//			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
//			break;
//		default:
//			continue;
//		}
//		//3 绑定vao
//		glBindVertexArray(geometry->getVao());
//		//4 执行绘制命令
//		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
//	}
//}