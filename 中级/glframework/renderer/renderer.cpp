#include "renderer.h"

Renderer::Renderer()
{
	mPhongShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	mWhiteShader = new Shader("assets/shaders/white.vert", "assets/shaders/white.frag");
	mDepthShader = new Shader("assets/shaders/depth.vert", "assets/shaders/depth.frag");
	mOpacityMaskShader = new Shader("assets/shaders/phongOpacityMask.vert", "assets/shaders/phongOpacityMask.frag");
	mScreenShader = new Shader("assets/shaders/screen.vert", "assets/shaders/screen.frag");
	mCubeShader = new Shader("assets/shaders/cube.vert", "assets/shaders/cube.frag");
}

Renderer::~Renderer()
{
}


void Renderer::setClearColor(glm::vec3 color) {
	glClearColor(color.r, color.g, color.b, 1.0);
}

void Renderer::setDepthState(Material* material) {
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
}

void Renderer::setPolygonOffsetState(Material* material) {
	//���polygonOffset
	if (material->mPolygonOffset)
	{
		glEnable(material->mPolygonOffsetType);
		glPolygonOffset(material->mFactor, material->mUnit);
	}
	else
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_POLYGON_OFFSET_LINE);
	}
}

void Renderer::setStencilState(Material* material) {
	if (material->mStencilTest)
	{
		glEnable(GL_STENCIL_TEST);
		glStencilOp(material->mSFail, material->mZFail, material->mZPass);
		glStencilMask(material->mStencilMask);
		glStencilFunc(material->mStencilFunc, material->mStencilRef, material->mStencilFuncMask);
	}
	else
	{
		glDisable(GL_STENCIL_TEST);
	}
}


void Renderer::setBlendState(Material* material) {
	if (material->mBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(material->mSFactor, material->mDFactor);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

void Renderer::setFaceCullingState(Material* material) {
	if (material->mFaceCulling)
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(material->mFrontFace);
		glCullFace(material->mCullFace);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}

void Renderer::render(Scene* scene, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight, unsigned int fbo) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);//�Ȱ󶨴�������FBO(��Ⱦ����fbo��)

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

	//Ĭ����ɫ���
	glDisable(GL_BLEND);//Ĭ�Ϲر�

	//2 ������
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//�����������
	mOpacityObjects.clear();
	mTransparentObjects.clear();
	projectObject(scene);

	std::sort(
		mTransparentObjects.begin(),
		mTransparentObjects.end(),
		[camera](const Mesh* a, const Mesh* b) {
			auto viewMatrix = camera->getViewMatrix();
			//1 ����A�����ϵ��Z
			auto modelMatrixA = a->getModelMatrix();
			auto worldPositionA = modelMatrixA * glm::vec4(0.0, 0.0, 0.0, 1.0);//���е���Ƭ�ʼ������ԭ���
			//��������ģ�ͱ任�����������ռ������
			auto cameraPositionA = viewMatrix * worldPositionA;

			//2 ����B�����ϵ��Z
			auto modelMatrixB = b->getModelMatrix();
			auto worldPositionB = modelMatrixB * glm::vec4(0.0, 0.0, 0.0, 1.0);//���е���Ƭ�ʼ������ԭ���
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;
		});

	//3 ��Ⱦ��������
	//����Ⱦ��͸��
	for (int i = 0; i < mOpacityObjects.size(); i++)
	{
		renderObject(mOpacityObjects[i], camera, dirLight, ambLight);
	}

	//����Ⱦ͸��
	for (int i = 0; i < mTransparentObjects.size(); i++)
	{
		renderObject(mTransparentObjects[i], camera, dirLight, ambLight);
	}
}

//��Ե���object������Ⱦ
void Renderer::renderObject(Object* object, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight) {
	//1 �ж���Mesh����Object �����Mesh����Ҫ��Ⱦ
	if (object-> getType() == ObjectType::Mesh)
	{
		//������Ⱦ����
		auto mesh = (Mesh*)object;
		auto geometry = mesh->mGeometry;

		//�����Ƿ�ӵ��ȫ�ֲ���
		Material* material = nullptr;
		if (mGlobalMaterial != nullptr)
		{
			material = mGlobalMaterial;
		}
		else
		{
			material = mesh->mMaterial;
		}

		//������Ⱦ״̬
		setDepthState(material);
		setPolygonOffsetState(material);
		setStencilState(material);
		setBlendState(material);
		setFaceCullingState(material);
		
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

			//͸����
			shader->setFloat("opacity", material->mOpacity);
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
		case MaterialType::OpacityMaskMaterial: {
			OpacityMaskMaterial* opacityMat = (OpacityMaskMaterial*)material;
			//�����������������Ԫ�ҹ�
			//diffuse�ɰ��֡����
			shader->setInt("sampler", 0);
			//������������Ԫ���йҹ�
			opacityMat->mDiffuse->bind();
			//opacityMask�ɰ��֡����
			shader->setInt("opacityMaskSampler", 1);
			opacityMat->mOpacityMask->bind();
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

			shader->setFloat("shiness", opacityMat->mShiness);
			//�����Ϣ����
			shader->setVector3("cameraPosition", camera->mPosition);

			//͸����
			shader->setFloat("opacity", material->mOpacity);
		}
			break;
		case MaterialType::ScreenMaterial: {
			ScreenMaterial* screenMat = (ScreenMaterial*)material;
			shader->setInt("screenTexSampler", 0);
			screenMat->mScreenTexture->bind();
			shader->setFloat("texWidth", 1600);//��д�� �������Ż�
			shader->setFloat("texHeight", 1200);
		}
			break;
		case MaterialType::CubeMaterial: {
			CubeMaterial* cubMat = (CubeMaterial*)material;
			mesh->setPosition(camera->mPosition);//�����ͬ���ƶ�
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
			shader->setInt("diffuse", 0);
			cubMat->mDiffuse->bind();

		}
			break;
		default:
			break;
		}
		//3 ��vao
		glBindVertexArray(geometry->getVao());
		//4 ִ�л�������
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
	}
}

//����ʵ����͸������
void Renderer::projectObject(Object* obj) {
	if (obj->getType() == ObjectType::Mesh)
	{
		Mesh* mesh = (Mesh*)obj;
		auto material = mesh->mMaterial;
		if (material->mBlend)
		{
			mTransparentObjects.push_back(mesh);
		}
		else
		{
			mOpacityObjects.push_back(mesh);
		}
	}

	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		projectObject(children[i]);
	}

}


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
	case MaterialType::OpacityMaskMaterial:
		result = mOpacityMaskShader;
		break;
	case MaterialType::ScreenMaterial:
		result = mScreenShader;
		break;
	case MaterialType::CubeMaterial:
		result = mCubeShader;
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
