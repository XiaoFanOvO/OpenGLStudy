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
	//检测深度状态
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
	//检测polygonOffset
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
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);//先绑定传进来的FBO(渲染到该fbo上)

	//1 设置当前帧绘制的时候,opengl的必要状态机参数
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);//保证每一帧能顺利清楚深度缓冲(否则如果最后一个mesh把深度写入关了,就没法clear,产生bug)
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);

	//开启测试 设置基本写入状态 打开模版测试写入
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0xff);//开启写入 保证了模版缓冲可以被清理

	//默认颜色混合
	glDisable(GL_BLEND);//默认关闭

	//2 清理画布
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//清空两个队列
	mOpacityObjects.clear();
	mTransparentObjects.clear();
	projectObject(scene);

	std::sort(
		mTransparentObjects.begin(),
		mTransparentObjects.end(),
		[camera](const Mesh* a, const Mesh* b) {
			auto viewMatrix = camera->getViewMatrix();
			//1 计算A的相机系的Z
			auto modelMatrixA = a->getModelMatrix();
			auto worldPositionA = modelMatrixA * glm::vec4(0.0, 0.0, 0.0, 1.0);//所有的面片最开始都是在原点的
			//乘上他的模型变换矩阵就是世界空间的坐标
			auto cameraPositionA = viewMatrix * worldPositionA;

			//2 计算B的相机系的Z
			auto modelMatrixB = b->getModelMatrix();
			auto worldPositionB = modelMatrixB * glm::vec4(0.0, 0.0, 0.0, 1.0);//所有的面片最开始都是在原点的
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;
		});

	//3 渲染两个队列
	//先渲染不透明
	for (int i = 0; i < mOpacityObjects.size(); i++)
	{
		renderObject(mOpacityObjects[i], camera, dirLight, ambLight);
	}

	//再渲染透明
	for (int i = 0; i < mTransparentObjects.size(); i++)
	{
		renderObject(mTransparentObjects[i], camera, dirLight, ambLight);
	}
}

//针对单个object进行渲染
void Renderer::renderObject(Object* object, Camera* camera, DirectionalLight* dirLight, AmbientLight* ambLight) {
	//1 判断是Mesh还是Object 如果是Mesh才需要渲染
	if (object-> getType() == ObjectType::Mesh)
	{
		//具体渲染流程
		auto mesh = (Mesh*)object;
		auto geometry = mesh->mGeometry;

		//考察是否拥有全局材质
		Material* material = nullptr;
		if (mGlobalMaterial != nullptr)
		{
			material = mGlobalMaterial;
		}
		else
		{
			material = mesh->mMaterial;
		}

		//设置渲染状态
		setDepthState(material);
		setPolygonOffsetState(material);
		setStencilState(material);
		setBlendState(material);
		setFaceCullingState(material);
		
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

			//透明度
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
			//将纹理采样器与纹理单元挂钩
			//diffuse蒙版的帧更新
			shader->setInt("sampler", 0);
			//将纹理与纹理单元进行挂钩
			opacityMat->mDiffuse->bind();
			//opacityMask蒙版的帧更新
			shader->setInt("opacityMaskSampler", 1);
			opacityMat->mOpacityMask->bind();
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

			shader->setFloat("shiness", opacityMat->mShiness);
			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);
		}
			break;
		case MaterialType::ScreenMaterial: {
			ScreenMaterial* screenMat = (ScreenMaterial*)material;
			shader->setInt("screenTexSampler", 0);
			screenMat->mScreenTexture->bind();
			shader->setFloat("texWidth", 1600);//先写死 后面再优化
			shader->setFloat("texHeight", 1200);
		}
			break;
		case MaterialType::CubeMaterial: {
			CubeMaterial* cubMat = (CubeMaterial*)material;
			mesh->setPosition(camera->mPosition);//与相机同步移动
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
		//3 绑定vao
		glBindVertexArray(geometry->getVao());
		//4 执行绘制命令
		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
	}
}

//区分实体与透明物体
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
