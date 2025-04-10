
#include "renderer.h"

Renderer::Renderer()
{
	mPhongShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	mWhiteShader = new Shader("assets/shaders/white.vert", "assets/shaders/white.frag");
	mDepthShader = new Shader("assets/shaders/depth.vert", "assets/shaders/depth.frag");
}

Renderer::~Renderer()
{
}


void Renderer::setClearColor(glm::vec3 color) {
	glClearColor(color.r, color.g, color.b, 1.0);
}


void Renderer::render(Scene* scene, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight) {
	//1 ���õ�ǰ֡���Ƶ�ʱ��,opengl�ı�Ҫ״̬������
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);//��֤ÿһ֡��˳�������Ȼ���(����������һ��mesh�����д�����,��û��clear,����bug)
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);

	//�������� ���û���д��״̬ ��ģ�����д��
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0xff);//����д�� ��֤��ģ�滺����Ա�����


	//2 ������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//3 ��scene�������ڵ� ��ʼ�ݹ���Ⱦ
	renderObject(scene, camera, dirLight, ambLight);
}

//��Ե���object������Ⱦ
void Renderer::renderObject(Object* object, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight) {
	//1 �ж���Mesh����Object �����Mesh����Ҫ��Ⱦ
	if (object-> getType() == ObjectType::Mesh)
	{
		//������Ⱦ����
		auto mesh = (Mesh*)object;
		auto geometry = mesh->mGeometry;
		auto material = mesh->mMaterial;

		//������״̬
		if (material->mDepthTest)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(material->mDepthFunc);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		if (material->mDepthWrite)
		{
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDepthMask(GL_FALSE);
		}

		//���polygonOffset
		if (material -> mPolygonOffset)
		{
			glEnable(material->mPolygonOffsetType);
			glPolygonOffset(material->mFactor, material->mUnit);
		}
		else
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
			glDisable(GL_POLYGON_OFFSET_LINE);
		}

		
		//1 ����ʹ���ĸ�shader
		Shader* shader = pickShader(material->mType);
		//2 ����shader��uniform
		shader->begin();
		switch (material->mType)
		{
		case MaterialType::PhongMaterial: {
			PhongMaterial* phongMat = (PhongMaterial*)material;
			//�����������������Ԫ�ҹ�
			//diffuse�ɰ��֡����
			shader->setInt("sampler", 0);
			//������������Ԫ���йҹ�
			phongMat->mDiffuse->bind();
			//�߹��ɰ��֡����
			//shader->setInt("specularMaskSampler", 1);
			//phongMat->mSpecularMask->bind();
			//MVP
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//��Դ������uniform���� 
			//directionalLight�ĸ���
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->mDirection);
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			
			shader->setFloat("shiness", phongMat->mShiness);
			//�����Ϣ����
			shader->setVector3("cameraPosition", camera->mPosition);
		}
			break;
		case MaterialType::WhiteMaterial:
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
			break;
		case MaterialType::DepthMaterial:
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
			shader->setFloat("near", camera->mNear);
			shader->setFloat("far", camera->mFar);
			break;
		default:
			break;
		}
		//3 ��vao
		glBindVertexArray(geometry->getVao());
		//4 ִ�л�������
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
	}


	//2 ������ǰObject���ӽڵ� ��ÿ���ӽڵ㶼��Ҫ����RenderObject(�ݹ�)
	auto children = object->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		if (i == 0)
		{
			//����A����
			glStencilFunc(GL_ALWAYS, 1, 0xff);//��β���
			glStencilMask(0xff); //����д��
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);//д��ģ�滺��
		}
		else if (i == 1)
		{
			// ����A�ı߽�
			glStencilFunc(GL_NOTEQUAL, 1, 0xff);
			glStencilMask(0x00);//�߽縲�ǵ�����Ӧ��д��1 
		}
		else if (i == 2)
		{
			//����B����
			glStencilFunc(GL_ALWAYS, 1, 0xff);//��β���
			glStencilMask(0xff); //����д��
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);//д��ģ�滺��
		}
		else if (i == 3)
		{
			// ����B�ı߽�
			glStencilFunc(GL_NOTEQUAL, 1, 0xff);
			glStencilMask(0x00);//�߽縲�ǵ�����Ӧ��д��1 
		}


		renderObject(children[i], camera, dirLight, ambLight);
	}

}

//void Renderer::render(const std::vector<Mesh*>& meshes, Camera* camera, DirectionalLight* dirLight, const std::vector<PointLight*>& pointLights, SpotLight* spotLight, AmbientLight* ambLight) {
//	//1 ���õ�ǰ֡���Ƶ�ʱ��,opengl�ı�Ҫ״̬������
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	//2 ������
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	//3 ����mesh���л���
//	for (int i = 0; i < meshes.size(); i++)
//	{
//		auto mesh = meshes[i];
//		auto geometry = mesh->mGeometry;
//		auto material = mesh->mMaterial;
//		//1 ����ʹ���ĸ�shader
//		Shader* shader = pickShader(material->mType);
//		//2 ����shader��uniform
//		shader->begin();
//		switch (material->mType)
//		{
//		case MaterialType::PhongMaterial: {
//			PhongMaterial* phongMat = (PhongMaterial*)material;
//			//�����������������Ԫ�ҹ�
//			//diffuse�ɰ��֡����
//			shader->setInt("sampler", 0);
//			//������������Ԫ���йҹ�
//			phongMat->mDiffuse->bind();
//			//�߹��ɰ��֡����
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
//			//��Դ������uniform���� 
//			//spotLight�ĸ���
//			shader->setVector3("spotLight.position", spotLight->getPosition());
//			shader->setVector3("spotLight.color", spotLight->mColor);
//			shader->setFloat("spotLight.specularIntensity", spotLight->mSpecularIntensity);
//			shader->setVector3("spotLight.targetDirection", spotLight->mTargetDirection);
//			//shader->setFloat("visibleAngle", glm::radians(spotLight->mVisibleAngle));//ע��ת��Ϊ���� 
//			shader->setFloat("spotLight.innerLine", glm::cos(glm::radians(spotLight->mInnerAngle)));
//			shader->setFloat("spotLight.outerLine", glm::cos(glm::radians(spotLight->mOuterAngle)));
//			//directionalLight�ĸ���
//			shader->setVector3("directionalLight.color", dirLight->mColor);
//			shader->setVector3("directionalLight.direction", dirLight->mDirection);
//			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
//			//pointLights�ĸ���
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
//			//�����Ϣ����
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
//		//3 ��vao
//		glBindVertexArray(geometry->getVao());
//		//4 ִ�л�������
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
	case MaterialType::DepthMaterial:
		result = mDepthShader;
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
//	//1 ���õ�ǰ֡���Ƶ�ʱ��,opengl�ı�Ҫ״̬������
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	//2 ������
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	//3 ����mesh���л���
//	for (int i = 0; i < meshes.size(); i++)
//	{
//		auto mesh = meshes[i];
//		auto geometry = mesh->mGeometry;
//		auto material = mesh->mMaterial;
//		//1 ����ʹ���ĸ�shader
//		Shader* shader = pickShader(material->mType);
//		//2 ����shader��uniform
//		shader->begin();
//		switch (material->mType)
//		{
//		case MaterialType::PhongMaterial: {
//			PhongMaterial* phongMat = (PhongMaterial*)material;
//			//�����������������Ԫ�ҹ�
//			//diffuse�ɰ��֡����
//			shader->setInt("sampler", 0);
//			//������������Ԫ���йҹ�
//			phongMat->mDiffuse->bind();
//			//�߹��ɰ��֡����
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
//			//��Դ������uniform���� 
//			shader->setVector3("lightDirection", dirLight->mDirection);
//			shader->setVector3("lightColor", dirLight->mColor);
//			shader->setFloat("specularIntensity", dirLight->mSpecularIntensity);
//			shader->setVector3("ambientColor", ambLight->mColor);
//			shader->setFloat("shiness", phongMat->mShiness);
//
//			//�����Ϣ����
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
//		//3 ��vao
//		glBindVertexArray(geometry->getVao());
//		//4 ִ�л�������
//		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
//	}
//}


//void Renderer::render(const std::vector<Mesh*>& meshes, Camera* camera, PointLight* pointLight, AmbientLight* ambLight) {
//	//1 ���õ�ǰ֡���Ƶ�ʱ��,opengl�ı�Ҫ״̬������
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	//2 ������
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	//3 ����mesh���л���
//	for (int i = 0; i < meshes.size(); i++)
//	{
//		auto mesh = meshes[i];
//		auto geometry = mesh->mGeometry;
//		auto material = mesh->mMaterial;
//		//1 ����ʹ���ĸ�shader
//		Shader* shader = pickShader(material->mType);
//		//2 ����shader��uniform
//		shader->begin();
//		switch (material->mType)
//		{
//		case MaterialType::PhongMaterial: {
//			PhongMaterial* phongMat = (PhongMaterial*)material;
//			//�����������������Ԫ�ҹ�
//			//diffuse�ɰ��֡����
//			shader->setInt("sampler", 0);
//			//������������Ԫ���йҹ�
//			phongMat->mDiffuse->bind();
//			//�߹��ɰ��֡����
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
//			//��Դ������uniform���� 
//			shader->setVector3("lightPosition", pointLight->getPosition());
//			shader->setVector3("lightColor", pointLight->mColor);
//			shader->setFloat("specularIntensity", pointLight->mSpecularIntensity);
//			shader->setVector3("ambientColor", ambLight->mColor);
//			shader->setFloat("shiness", phongMat->mShiness);
//			shader->setFloat("k2", pointLight->mK2);
//			shader->setFloat("k1", pointLight->mK1);
//			shader->setFloat("kc", pointLight->mKc);
//
//			//�����Ϣ����
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
//		//3 ��vao
//		glBindVertexArray(geometry->getVao());
//		//4 ִ�л�������
//		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
//	}
//}