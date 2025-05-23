#include "renderer.h"
#include <iostream>
#include "../material/phongMaterial.h"
#include "../material/whiteMaterial.h"
#include "../material/opacityMaskMaterial.h"
#include "../material/screenMaterial.h"
#include "../material/cubeMaterial.h"
#include "../material/phongEnvMaterial.h"
#include "../material/phongInstanceMaterial.h"
#include "../material/grassInstanceMaterial.h"
#include "../material/advanced/phongNormalMaterial.h"
#include "../material/advanced/phongParallaxMaterial.h"
#include "../material/advanced/phongShadowMaterial.h"
#include "../material/advanced/phongCSMShadowMaterial.h"
#include "../mesh/instancedMesh.h"
#include "../../application/camera/orthographicCamera.h"
#include "../light/shadow/directionalLightShadow.h"
#include "../light/shadow/directionalLightCSMShadow.h"
#include <string>//stl string
#include <algorithm>


Renderer::Renderer() {
	//mPhongShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	mWhiteShader = new Shader("assets/shaders/white.vert", "assets/shaders/white.frag");
	mDepthShader = new Shader("assets/shaders/depth.vert", "assets/shaders/depth.frag");
	mOpacityMaskShader = new Shader("assets/shaders/phongOpacityMask.vert", "assets/shaders/phongOpacityMask.frag");
	mScreenShader = new Shader("assets/shaders/screen.vert", "assets/shaders/screen.frag");
	mCubeShader = new Shader("assets/shaders/cube.vert", "assets/shaders/cube.frag");
	mPhongEnvShader = new Shader("assets/shaders/phongEnv.vert", "assets/shaders/phongEnv.frag");
	mPhongInstanceShader = new Shader("assets/shaders/phongInstance.vert", "assets/shaders/phongInstance.frag");
	mGrassInstanceShader = new Shader("assets/shaders/grassInstance.vert", "assets/shaders/grassInstance.frag");

	mPhongNormalShader = new Shader("assets/shaders/advanced/phongNormal.vert", "assets/shaders/advanced/phongNormal.frag");
	mPhongParallaxShader = new Shader("assets/shaders/advanced/phongParallax.vert", "assets/shaders/advanced/phongParallax.frag");
	mPhongShader = new Shader("assets/shaders/advanced/phong.vert", "assets/shaders/advanced/phong.frag");
	mShadowShader = new Shader("assets/shaders/advanced/shadow.vert", "assets/shaders/advanced/shadow.frag");
	mPhongShadowShader = new Shader("assets/shaders/advanced/phongShadow.vert", "assets/shaders/advanced/phongShadow.frag");
	mPhongCSMShadowShader = new Shader("assets/shaders/advanced/phongCSMShadow.vert", "assets/shaders/advanced/phongCSMShadow.frag");
}

Renderer::~Renderer() {

}

void Renderer::setClearColor(glm::vec3 color) {
	glClearColor(color.r, color.g, color.b, 1.0);
}


void Renderer::render(
	Scene* scene,
	Camera* camera,
	DirectionalLight* dirLight,
	AmbientLight* ambLight,
	unsigned int fbo
) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	////1 设置当前帧绘制的时候，opengl的必要状态机参数
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_POLYGON_OFFSET_LINE);


	//开启测试、设置基本写入状态，打开模板测试写入
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0xFF);//保证了模板缓冲可以被清理

	//默认颜色混合
	glDisable(GL_BLEND);

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

			//1 计算a的相机系的Z
			auto modelMatrixA = a->getModelMatrix();
			auto worldPositionA = modelMatrixA * glm::vec4(0.0, 0.0, 0.0, 1.0);
			auto cameraPositionA = viewMatrix * worldPositionA;

			//2 计算b的相机系的Z
			auto modelMatrixB = b->getModelMatrix();
			auto worldPositionB = modelMatrixB * glm::vec4(0.0, 0.0, 0.0, 1.0);
			auto cameraPositionB = viewMatrix * worldPositionB;

			return cameraPositionA.z < cameraPositionB.z;
		}
	);

	//渲染shadowmap
	//需要做好备份工作,特别是fbo和viewport,后面要恢复,否则影响到后面的渲染
	//这里先做不透明物体的阴影
	//还要做排除,postprocess不做shadowmap
	//renderShadowMap(mOpacityObjects, dirLight, dirLight->mShadow->mRenderTarget);

	//3 渲染两个队列
	for (int i = 0; i < mOpacityObjects.size(); i++) {
		renderObject(mOpacityObjects[i], camera, dirLight, ambLight);
	}

	for (int i = 0; i < mTransparentObjects.size(); i++) {
		renderObject(mTransparentObjects[i], camera, dirLight, ambLight);
	}
}

void Renderer::projectObject(Object* obj) {
	if (obj->getType() == ObjectType::Mesh || obj->getType() == ObjectType::InstancedMesh) {
		Mesh* mesh = (Mesh*)obj;
		auto material = mesh->mMaterial;
		if (material->mBlend) {
			mTransparentObjects.push_back(mesh);
		}
		else {
			mOpacityObjects.push_back(mesh);
		}
	}

	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++) {
		projectObject(children[i]);
	}
}

Shader* Renderer::pickShader(MaterialType type) {
	Shader* result = nullptr;

	switch (type) {
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
	case MaterialType::PhongEnvMaterial:
		result = mPhongEnvShader;
		break;
	case MaterialType::PhongInstanceMaterial:
		result = mPhongInstanceShader;
		break;
	case MaterialType::GrassInstanceMaterial:
		result = mGrassInstanceShader;
		break;
	case MaterialType::PhongNormalMaterial:
		result = mPhongNormalShader;
		break;
	case MaterialType::PhongParallaxMaterial:
		result = mPhongParallaxShader;
		break;
	case MaterialType::PhongShadowMaterial:
		result = mPhongShadowShader;
		break;
	case MaterialType::PhongCSMShadowMaterial:
		result = mPhongCSMShadowShader;
		break;
	default:
		std::cout << "Unknown material type to pick shader" << std::endl;
		break;
	}

	return result;
}

void Renderer::renderShadowMap(const std::vector<Mesh*>& meshes, DirectionalLight* dirLight, Framebuffer* fbo) {
	//1 确保现在的绘制不是postProcessPass的绘制,如果是,则不执行渲染
	bool isPostProcessPass = true;//通过查找screenmaterial
	for (int i = 0; i < meshes.size(); i++)
	{
		auto mesh = meshes[i];
		if (mesh->mMaterial->mType != MaterialType::ScreenMaterial)
		{
			isPostProcessPass = false;
			break;
		}
	}

	if (isPostProcessPass)
	{
		return;//后处理pass不做阴影 
	}

	//2 保存原始状态,绘制shadowMap完毕后,要恢复原始状态
	GLint preFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &preFbo);//查找当前绑定的FBO并赋值

	GLint preViewport[4];
	glGetIntegerv(GL_VIEWPORT, preViewport);//查找当前视口并赋值


	//3 设置shadowPass绘制的时候所需的状态
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	//绑定深度图需要的FBO和视口
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->mFBO);
	glViewport(0, 0, fbo->mWidth, fbo->mHeight);

	//4 开始绘制
	glClear(GL_DEPTH_BUFFER_BIT);
	DirectionalLightShadow* dirShadow = (DirectionalLightShadow*)dirLight->mShadow;
	auto lightMatrix = dirShadow->getLightMatrix(dirLight->getModelMatrix());
	mShadowShader->begin();
	mShadowShader->setMatrix4x4("lightMatrix", lightMatrix);//所有mesh共有
	for (int i = 0; i < meshes.size(); i++)
	{
		auto mesh = meshes[i];
		auto geometry = mesh->mGeometry;

		glBindVertexArray(geometry->getVao());
		mShadowShader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());

		//4 执行绘制命令
		if (mesh->getType() == ObjectType::InstancedMesh) {
			InstancedMesh* im = (InstancedMesh*)mesh;
			glDrawElementsInstanced(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0, im->mInstanceCount);
		}
		else {
			glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
		}
	}

	mShadowShader->end();


	//5 恢复状态
	glBindFramebuffer(GL_FRAMEBUFFER, preFbo);
	glViewport(preViewport[0], preViewport[1], preViewport[2], preViewport[3]);
}

//glm::mat4 Renderer::getLightMatrix(DirectionalLight* dirLight) {
//	//1 viewMatrix
//	auto lightViewMatrix = glm::inverse(dirLight->getModelMatrix());
//	//2 projection(正交投影)
//	float size = 6.0f;
//	auto lightCamera = new OrthographicCamera(-size, size, -size, size, 0.1f, 80);
//	auto lightProjectionMatrix = lightCamera->getProjectionMatrix();
//
//	//3 求lightMatrix并返回
//	return lightProjectionMatrix * lightViewMatrix;
//}


//针对单个object进行渲染
void Renderer::renderObject(
	Object* object,
	Camera* camera,
	DirectionalLight* dirLight,
	AmbientLight* ambLight
) {
	//判断是Mesh还是Object，如果是Mesh需要渲染
	if (object->getType() == ObjectType::Mesh || object->getType() == ObjectType::InstancedMesh) {
		auto mesh = (Mesh*)object;
		auto geometry = mesh->mGeometry;

		//考察是否拥有全局材质
		Material* material = nullptr;
		if (mGlobalMaterial != nullptr) {
			material = mGlobalMaterial;
		}
		else {
			material = mesh->mMaterial;
		}

		//设置渲染状态
		setDepthState(material);
		setPolygonOffsetState(material);
		setStencilState(material);
		setBlendState(material);
		setFaceCullingState(material);

		//1 决定使用哪个Shader 
		Shader* shader = pickShader(material->mType);

		//2 更新shader的uniform
		shader->begin();

		switch (material->mType) {
		case MaterialType::PhongMaterial: {
			PhongMaterial* phongMat = (PhongMaterial*)material;

			//diffuse贴图帧更新
			//将纹理采样器与纹理单元进行挂钩
			shader->setInt("sampler", 0);
			//将纹理与纹理单元进行挂钩
			phongMat->mDiffuse->bind();

			//高光蒙版的帧更新
			/*shader->setInt("specularMaskSampler", 1);
			phongMat->mSpecularMask->bind();*/

			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//光源参数的uniform更新
			//directionalLight 的更新
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->getDirection());
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("directionalLight.intensity", dirLight->mIntensity);

			shader->setFloat("shiness", phongMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);

		}
										break;
		case MaterialType::OpacityMaskMaterial: {
			OpacityMaskMaterial* opacityMat = (OpacityMaskMaterial*)material;

			//diffuse贴图帧更新
			//将纹理采样器与纹理单元进行挂钩
			shader->setInt("sampler", 0);
			//将纹理与纹理单元进行挂钩
			opacityMat->mDiffuse->bind();

			//opacityMask的帧更新
			shader->setInt("opacityMaskSampler", 1);
			opacityMat->mOpacityMask->bind();

			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//光源参数的uniform更新
			//directionalLight 的更新
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->getDirection());
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);


			shader->setFloat("shiness", opacityMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);
		}
											  break;
		case MaterialType::WhiteMaterial: {
			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
		}
										break;
		case MaterialType::DepthMaterial: {
			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			shader->setFloat("near", camera->mNear);
			shader->setFloat("far", camera->mFar);
		}
										break;
		case MaterialType::ScreenMaterial: {
			ScreenMaterial* screenMat = (ScreenMaterial*)material;
			shader->setInt("screenTexSampler", 0);

			//凑合了一下
			shader->setFloat("texWidth", 1600);
			shader->setFloat("texHeight", 1200);

			screenMat->mScreenTexture->bind();
		}
										 break;
		case MaterialType::CubeMaterial: {
			CubeMaterial* cubeMat = (CubeMaterial*)material;
			mesh->setPosition(camera->mPosition);
			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			shader->setInt("cubeSampler", 0);
			cubeMat->mDiffuse->bind();
		}
									   break;
		case MaterialType::PhongEnvMaterial: {
			PhongEnvMaterial* phongMat = (PhongEnvMaterial*)material;

			shader->setInt("sampler", 0);
			phongMat->mDiffuse->bind();

			shader->setInt("envSampler", 1);
			phongMat->mEnv->bind();

			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//光源参数的uniform更新
			//directionalLight 的更新
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->getDirection());
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);


			shader->setFloat("shiness", phongMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);
		}
										   break;
		case MaterialType::PhongInstanceMaterial: {
			PhongInstanceMaterial* phongMat = (PhongInstanceMaterial*)material;
			InstancedMesh* im = (InstancedMesh*)mesh;

			//diffuse贴图帧更新
			//将纹理采样器与纹理单元进行挂钩
			shader->setInt("sampler", 0);
			//将纹理与纹理单元进行挂钩
			phongMat->mDiffuse->bind();

			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//光源参数的uniform更新
			//directionalLight 的更新
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->getDirection());
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);


			shader->setFloat("shiness", phongMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);
			 
			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);
		}
												break;
		case MaterialType::GrassInstanceMaterial: {
			GrassInstanceMaterial* grassMat = (GrassInstanceMaterial*)material;
			InstancedMesh* im = (InstancedMesh*)mesh;

			im->sortMatrices(camera->getViewMatrix());
			im->updateMatrices();

			shader->setInt("sampler", 0);
			grassMat->mDiffuse->bind();

			shader->setInt("opacityMask", 1);
			grassMat->mOpacityMask->bind();

			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			/*	auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
				shader->setMatrix3x3("normalMatrix", normalMatrix);*/

				//光源参数的uniform更新
				//directionalLight 的更新
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->getDirection());
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);


			shader->setFloat("shiness", grassMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);

			//贴图特性
			shader->setFloat("uvScale", grassMat->mUVScale);
			shader->setFloat("brightness", grassMat->mBrightness);
			shader->setFloat("time", glfwGetTime());
			//风力
			shader->setFloat("windScale", grassMat->mWindScale);
			shader->setFloat("phaseScale", grassMat->mPhaseScale);
			shader->setVector3("windDirection", grassMat->mWindDirection);

			//云层
			shader->setInt("cloudMask", 2);
			grassMat->mCloudMask->bind();
			shader->setVector3("cloudWhiteColor", grassMat->mCloudWhiteColor);
			shader->setVector3("cloudBlackColor", grassMat->mCloudBlackColor);
			shader->setFloat("cloudUVScale", grassMat->mCloudUVScale);
			shader->setFloat("cloudSpeed", grassMat->mCloudSpeed);
			shader->setFloat("cloudLerp", grassMat->mCloudLerp);
		}
												break;
		case MaterialType::PhongNormalMaterial: {
			PhongNormalMaterial* phongMat = (PhongNormalMaterial*)material;

			//diffuse贴图帧更新
			shader->setInt("sampler", 0);
			phongMat->mDiffuse->bind();

			//法线贴图
			shader->setInt("normalMapSampler", 1);
			phongMat->mNormalMap->bind();


			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//光源参数的uniform更新
			//directionalLight 的更新
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->getDirection());
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("directionalLight.intensity", dirLight->mIntensity);

			shader->setFloat("shiness", phongMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);

		}
											  break;
		case MaterialType::PhongParallaxMaterial: {
			PhongParallaxMaterial* phongMat = (PhongParallaxMaterial*)material;

			//diffuse贴图帧更新
			shader->setInt("sampler", 0);
			phongMat->mDiffuse->bind();

			//法线贴图
			shader->setInt("normalMapSampler", 1);
			phongMat->mNormalMap->bind();

			//视差贴图
			shader->setInt("parallaxMapSampler", 2);
			phongMat->mParallaxMap->bind();


			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//光源参数的uniform更新
			//directionalLight 的更新
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->getDirection());
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("directionalLight.intensity", dirLight->mIntensity);

			shader->setFloat("shiness", phongMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", material->mOpacity);

			shader->setFloat("heightScale", phongMat->mHeightScale);
			shader->setFloat("layerNum", phongMat->mLayerNum);

		}
												break;
		case MaterialType::PhongShadowMaterial: {
			PhongShadowMaterial* phongShadowMat = (PhongShadowMaterial*)material;
			DirectionalLightShadow* dirShadow = (DirectionalLightShadow*)dirLight->mShadow;
			//diffuse贴图帧更新
			//将纹理采样器与纹理单元进行挂钩
			shader->setInt("sampler", 0);
			//将纹理与纹理单元进行挂钩
			phongShadowMat->mDiffuse->bind();

			//shadow相关
			shader->setInt("shadowMapSampler", 1);
			dirShadow->mRenderTarget->mDepthAttachment->setUnit(1);
			dirShadow->mRenderTarget->mDepthAttachment->bind();


			shader->setMatrix4x4("lightMatrix", dirShadow->getLightMatrix(dirLight->getModelMatrix()));
			shader->setMatrix4x4("lightViewMatrix", glm::inverse(dirLight->getModelMatrix()));

			//高光蒙版的帧更新
			/*shader->setInt("specularMaskSampler", 1);
			phongMat->mSpecularMask->bind();*/

			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//光源参数的uniform更新
			//directionalLight 的更新
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->getDirection());
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("directionalLight.intensity", dirLight->mIntensity);

			shader->setFloat("shiness", phongShadowMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", phongShadowMat->mOpacity);
			//bias
			shader->setFloat("bias", dirShadow->mBias);
			shader->setFloat("diskTightness", dirShadow->mDiskTightness);
			shader->setFloat("pcfRadius", dirShadow->mPcfRadius);
			shader->setFloat("lightSize", dirShadow->mLightSize);

			OrthographicCamera* camera = (OrthographicCamera*)dirShadow->mCamera;
			float frustum = camera->mR - camera->mL;
			float nearPlane = camera->mNear;
			shader->setFloat("frustum", frustum);
			shader->setFloat("nearPlane", nearPlane);

		}
										break;
		case MaterialType::PhongCSMShadowMaterial: {
			PhongCSMShadowMaterial* phongShadowMat = (PhongCSMShadowMaterial*)material;
			DirectionalLightCSMShadow* dirCSMShadow = (DirectionalLightCSMShadow*)dirLight->mShadow;
			//diffuse贴图帧更新
			//将纹理采样器与纹理单元进行挂钩
			shader->setInt("sampler", 0);
			//将纹理与纹理单元进行挂钩
			phongShadowMat->mDiffuse->bind();

			shader->setInt("csmLayerCount", dirCSMShadow->mLayerCount);

			std::vector<float> layers;
			dirCSMShadow->generateCascadeLayers(layers, camera->mNear, camera->mFar);
			shader->setFloatArray("csmLayers", layers.data(), layers.size());

			//shadow相关
			/*shader->setInt("shadowMapSampler", 1);
			dirShadow->mRenderTarget->mDepthAttachment->setUnit(1);
			dirShadow->mRenderTarget->mDepthAttachment->bind();*/


			/*shader->setMatrix4x4("lightMatrix", dirShadow->getLightMatrix(dirLight->getModelMatrix()));
			shader->setMatrix4x4("lightViewMatrix", glm::inverse(dirLight->getModelMatrix()));*/

			//高光蒙版的帧更新 
			/*shader->setInt("specularMaskSampler", 1);
			phongMat->mSpecularMask->bind();*/

			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//光源参数的uniform更新
			//directionalLight 的更新
			shader->setVector3("directionalLight.color", dirLight->mColor);
			shader->setVector3("directionalLight.direction", dirLight->getDirection());
			shader->setFloat("directionalLight.specularIntensity", dirLight->mSpecularIntensity);
			shader->setFloat("directionalLight.intensity", dirLight->mIntensity);

			shader->setFloat("shiness", phongShadowMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", phongShadowMat->mOpacity);
			//bias
			/*shader->setFloat("bias", dirShadow->mBias);
			shader->setFloat("diskTightness", dirShadow->mDiskTightness);
			shader->setFloat("pcfRadius", dirShadow->mPcfRadius);
			shader->setFloat("lightSize", dirShadow->mLightSize);*/

			/*OrthographicCamera* camera = (OrthographicCamera*)dirShadow->mCamera;
			float frustum = camera->mR - camera->mL;
			float nearPlane = camera->mNear;
			shader->setFloat("frustum", frustum);
			shader->setFloat("nearPlane", nearPlane);*/

		}
											  break;
		default:
			break;
		}

		//3 绑定vao
		glBindVertexArray(geometry->getVao());

		//4 执行绘制命令
		if (object->getType() == ObjectType::InstancedMesh) {
			InstancedMesh* im = (InstancedMesh*)mesh;
			glDrawElementsInstanced(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0, im->mInstanceCount);
		}
		else {
			glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
		}
	}
}

void Renderer::setDepthState(Material* material) {
	if (material->mDepthTest) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(material->mDepthFunc);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}

	if (material->mDepthWrite) {
		glDepthMask(GL_TRUE);
	}
	else {
		glDepthMask(GL_FALSE);
	}
}
void Renderer::setPolygonOffsetState(Material* material) {
	if (material->mPolygonOffset) {
		glEnable(material->mPolygonOffsetType);
		glPolygonOffset(material->mFactor, material->mUnit);
	}
	else {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_POLYGON_OFFSET_LINE);
	}
}

void Renderer::setStencilState(Material* material) {
	if (material->mStencilTest) {
		glEnable(GL_STENCIL_TEST);

		glStencilOp(material->mSFail, material->mZFail, material->mZPass);
		glStencilMask(material->mStencilMask);
		glStencilFunc(material->mStencilFunc, material->mStencilRef, material->mStencilFuncMask);

	}
	else {
		glDisable(GL_STENCIL_TEST);
	}
}

void Renderer::setBlendState(Material* material) {
	if (material->mBlend) {
		glEnable(GL_BLEND);
		glBlendFunc(material->mSFactor, material->mDFactor);
	}
	else {
		glDisable(GL_BLEND);
	}
}

void Renderer::setFaceCullingState(Material* material) {
	if (material->mFaceCulling) {
		glEnable(GL_CULL_FACE);
		glFrontFace(material->mFrontFace);
		glCullFace(material->mCullFace);
	}
	else {
		glDisable(GL_CULL_FACE);
	}
}



//
//void Renderer::render(
//	const std::vector<Mesh*>& meshes,
//	Camera* camera,
//	PointLight* pointLight,
//	AmbientLight* ambLight
//) {
//	//1 设置当前帧绘制的时候，opengl的必要状态机参数
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//
//	//2 清理画布 
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	//3 遍历mesh进行绘制
//	for (int i = 0; i < meshes.size(); i++) {
//		auto mesh = meshes[i];
//		auto geometry = mesh->mGeometry;
//		auto material = mesh->mMaterial;
//
//		//1 决定使用哪个Shader 
//		Shader* shader = pickShader(material->mType);
//
//		//2 更新shader的uniform
//		shader->begin();
//
//		switch (material->mType) {
//		case MaterialType::PhongMaterial: {
//			PhongMaterial* phongMat = (PhongMaterial*)material;
//
//			//diffuse贴图帧更新
//			//将纹理采样器与纹理单元进行挂钩
//			shader->setInt("sampler", 0);
//			//将纹理与纹理单元进行挂钩
//			phongMat->mDiffuse->bind();
//
//			//高光蒙版的帧更新
//			shader->setInt("specularMaskSampler", 1);
//			phongMat->mSpecularMask->bind();
//
//			//mvp
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
//			shader->setFloat("k2", pointLight->mK2);
//			shader->setFloat("k1", pointLight->mK1);
//			shader->setFloat("kc", pointLight->mKc);
//
//			shader->setFloat("shiness", phongMat->mShiness);
//
//			shader->setVector3("ambientColor", ambLight->mColor);
//
//			//相机信息更新
//			shader->setVector3("cameraPosition", camera->mPosition);
//
//		}
//										break;
//		case MaterialType::WhiteMaterial: {
//			//mvp
//			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
//			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
//			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
//		}
//										break;
//		default:
//			continue;
//		}
//
//		//3 绑定vao
//		glBindVertexArray(geometry->getVao());
//
//		//4 执行绘制命令
//		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
//	}
//}



//void Renderer::render(
//	const std::vector<Mesh*>& meshes,
//	Camera* camera,
//	DirectionalLight* dirLight,
//	AmbientLight* ambLight
//) {
//	//1 设置当前帧绘制的时候，opengl的必要状态机参数
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//
//	//2 清理画布 
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	//3 遍历mesh进行绘制
//	for (int i = 0; i < meshes.size(); i++) {
//		auto mesh = meshes[i];
//		auto geometry = mesh->mGeometry;
//		auto material = mesh->mMaterial;
//
//		//1 决定使用哪个Shader 
//		Shader* shader = pickShader(material->mType);
//
//		//2 更新shader的uniform
//		shader->begin();
//
//		switch (material->mType) {
//		case MaterialType::PhongMaterial: {
//			PhongMaterial* phongMat = (PhongMaterial*)material;
//
//			//diffuse贴图帧更新
//			//将纹理采样器与纹理单元进行挂钩
//			shader->setInt("sampler", 0);
//			//将纹理与纹理单元进行挂钩
//			phongMat->mDiffuse->bind();
//
//			//高光蒙版的帧更新
//			shader->setInt("specularMaskSampler", 1);
//			phongMat->mSpecularMask->bind();
//
//			//mvp
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
//
//			shader->setFloat("shiness", phongMat->mShiness);
//
//			shader->setVector3("ambientColor", ambLight->mColor);
//
//			//相机信息更新
//			shader->setVector3("cameraPosition", camera->mPosition);
//
//		}
//										break;
//		case MaterialType::WhiteMaterial: {
//			//mvp
//			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
//			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
//			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
//		}
//										break;
//		default:
//			continue;
//		}
//
//		//3 绑定vao
//		glBindVertexArray(geometry->getVao());
//
//		//4 执行绘制命令
//		glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
//	}
//}
