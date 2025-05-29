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
#include "../material/advanced/phongPointShadowMaterial.h"
#include "../mesh/instancedMesh.h"
#include "../../application/camera/orthographicCamera.h"
#include "../light/shadow/directionalLightShadow.h"
#include "../light/shadow/directionalLightCSMShadow.h"
#include "../light/shadow/pointLightShadow.h"
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

	mShadowDistanceShader = new Shader("assets/shaders/advanced/shadowDistance.vert", "assets/shaders/advanced/shadowDistance.frag");
	mPhongPointShadowShader = new Shader("assets/shaders/advanced/phongPointShadow.vert", "assets/shaders/advanced/phongPointShadow.frag");

}

Renderer::~Renderer() {

}

void Renderer::setClearColor(glm::vec3 color) {
	glClearColor(color.r, color.g, color.b, 1.0);
}

void Renderer::msaaResolve(Framebuffer* src, Framebuffer* dst) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->mFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->mFBO);
	//将颜色从src复制到dst
	glBlitFramebuffer(0, 0, src->mWidth, src->mHeight, 0, 0, dst->mWidth, dst->mHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Renderer::render(
	Scene* scene,
	Camera* camera,
	PointLight* pointLight,
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
	renderShadowMap(camera, mOpacityObjects, pointLight);

	//3 渲染两个队列
	for (int i = 0; i < mOpacityObjects.size(); i++) {
		renderObject(mOpacityObjects[i], camera, pointLight, ambLight);
	}

	for (int i = 0; i < mTransparentObjects.size(); i++) {
		renderObject(mTransparentObjects[i], camera, pointLight, ambLight);
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
	case MaterialType::PhongPointShadowMaterial:
		result = mPhongPointShadowShader;
		break;
	default:
		std::cout << "Unknown material type to pick shader" << std::endl;
		break;
	}

	return result;
}

void Renderer::renderShadowMap(Camera* camera, const std::vector<Mesh*>& meshes, PointLight* pointLight) {
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

	PointLightShadow* pointShadow = (PointLightShadow*)pointLight->mShadow;
	auto lightMatrices = pointShadow->getLightMatrices(pointLight->getPosition());

	////绑定深度图需要的FBO和视口
	glBindFramebuffer(GL_FRAMEBUFFER, pointShadow->mRenderTarget->mFBO);//绑定一个FBO 但是循环了六次
	glViewport(0, 0, pointShadow->mRenderTarget->mWidth, pointShadow->mRenderTarget->mHeight);

	//4 循环为Cubemap每个面渲染shadowmap
	for (int i = 0; i < 6; i++) {
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			pointShadow->mRenderTarget->mDepthAttachment->getTexture(),
			0
		);
		glClear(GL_DEPTH_BUFFER_BIT);//!!!别忘了
		mShadowDistanceShader->begin();
		mShadowDistanceShader->setMatrix4x4("lightMatrix", lightMatrices[i]);
		mShadowDistanceShader->setFloat("far", pointShadow->mCamera->mFar);//是阴影相机不是玩家相机!
		mShadowDistanceShader->setVector3("lightPosition", pointLight->getPosition());
		for (int i = 0; i < meshes.size(); i++) {
			auto mesh = meshes[i];
			auto geometry = mesh->mGeometry;

			glBindVertexArray(geometry->getVao());
			mShadowDistanceShader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());

			if (mesh->getType() == ObjectType::InstancedMesh) {
				InstancedMesh* im = (InstancedMesh*)mesh;
				glDrawElementsInstanced(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0, im->mInstanceCount);
			}
			else {
				glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
			}
		}

		mShadowDistanceShader->end();
	}

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
	PointLight* pointLight,
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
		case MaterialType::WhiteMaterial: {
			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
		}
										break;
		case MaterialType::ScreenMaterial: {
			ScreenMaterial* screenMat = (ScreenMaterial*)material;
			shader->setInt("screenTexSampler", 0);

			//凑合了一下
			shader->setFloat("texWidth", 1600);
			shader->setFloat("texHeight", 1200);
			shader->setFloat("exposure", screenMat->mExposure);

			screenMat->mScreenTexture->bind();
		}
										 break;
		case MaterialType::PhongPointShadowMaterial: {
			PhongPointShadowMaterial* pointShadowMat = (PhongPointShadowMaterial*)material;
			PointLightShadow* pointShadow = (PointLightShadow*)pointLight->mShadow;
			//diffuse贴图帧更新
			//将纹理采样器与纹理单元进行挂钩
			shader->setInt("sampler", 0);
			//将纹理与纹理单元进行挂钩
			pointShadowMat->mDiffuse->bind();

			//shadow相关
			shader->setInt("shadowMapSampler", 1);
			pointShadow->mRenderTarget->mDepthAttachment->setUnit(1);
			pointShadow->mRenderTarget->mDepthAttachment->bind();

			//bias
			shader->setFloat("bias", pointShadow->mBias);
			shader->setFloat("diskTightness", pointShadow->mDiskTightness);
			shader->setFloat("pcfRadius", pointShadow->mPcfRadius);
			shader->setFloat("far", pointShadow->mCamera->mFar);


			//mvp
			shader->setMatrix4x4("modelMatrix", mesh->getModelMatrix());
			shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
			shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());

			auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh->getModelMatrix())));
			shader->setMatrix3x3("normalMatrix", normalMatrix);

			//光源参数的uniform更新
			//pointLight 的更新
			shader->setVector3("pointLight.color", pointLight->mColor);
			shader->setFloat("pointLight.specularIntensity", pointLight->mSpecularIntensity);
			shader->setVector3("pointLight.position", pointLight->getPosition());

			shader->setFloat("pointLight.k2", pointLight->mK2);
			shader->setFloat("pointLight.k1", pointLight->mK1);
			shader->setFloat("pointLight.kc", pointLight->mKc);

			shader->setFloat("shiness", pointShadowMat->mShiness);

			shader->setVector3("ambientColor", ambLight->mColor);

			//相机信息更新
			shader->setVector3("cameraPosition", camera->mPosition);

			//透明度
			shader->setFloat("opacity", pointShadowMat->mOpacity);
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

