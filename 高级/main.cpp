#include <iostream>

#include "glframework/core.h"
#include "glframework/shader.h"
#include <string>
#include <assert.h>//断言
#include "wrapper/checkError.h"
#include "application/Application.h"
#include "glframework/texture.h"

//引入相机+控制器
#include "application/camera/perspectiveCamera.h"
#include "application/camera/orthographicCamera.h"
#include "application/camera/trackBallCameraControl.h"
#include "application/camera/GameCameraControl.h"

#include "glframework/geometry.h"
#include "glframework/material/phongMaterial.h"
#include "glframework/material/whiteMaterial.h"
#include "glframework/material/depthMaterial.h"
#include "glframework/material/opacityMaskMaterial.h"
#include "glframework/material/screenMaterial.h"
#include "glframework/material/cubeMaterial.h"
#include "glframework/material/phongEnvMaterial.h"
#include "glframework/material/phongInstanceMaterial.h"
#include "glframework/material/grassInstanceMaterial.h"
#include "glframework/material/advanced/phongNormalMaterial.h"
#include "glframework/material/advanced/phongParallaxMaterial.h"
#include "glframework/material/advanced/phongShadowMaterial.h"
#include "glframework/material/advanced/phongCSMShadowMaterial.h"
#include "glframework/material/advanced/phongPointShadowMaterial.h"


#include "glframework/mesh/mesh.h"
#include "glframework/mesh/instancedMesh.h"
#include "glframework/renderer/renderer.h"
#include "glframework/light/pointLight.h"
#include "glframework/light/spotLight.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glframework/scene.h"
#include "application/assimpLoader.h"

#include "glframework/framebuffer/framebuffer.h"

#include "application/assimpInstanceLoader.h"

Renderer* renderer = nullptr;
Scene* sceneOff = nullptr;
Scene* scene = nullptr;
Framebuffer* fbo = nullptr;

ScreenMaterial* screenMat = nullptr;

Mesh* upPlane = nullptr;

int WIDTH = 2560;
int HEIGHT = 1440;

PointLight* pointLight = nullptr;

AmbientLight* ambLight = nullptr;

Camera* camera = nullptr;
GameCameraControl* cameraControl = nullptr;

glm::vec3 clearColor{};

void OnResize(int width, int height) {
	GL_CALL(glViewport(0, 0, width, height));
}

void OnKey(int key, int action, int mods) {
	cameraControl->onKey(key, action, mods);
}

//鼠标按下/抬起
void OnMouse(int button, int action, int mods) {
	double x, y;
	glApp->getCursorPosition(&x, &y);
	cameraControl->onMouse(button, action, x, y);
}

//鼠标移动
void OnCursor(double xpos, double ypos) {
	cameraControl->onCursor(xpos, ypos);
}

//鼠标滚轮
void OnScroll(double offset) {
	cameraControl->onScroll(offset);
}


void prepare() {
	fbo = Framebuffer::createHDRFbo(WIDTH, HEIGHT);
	//fbo = new Framebuffer(WIDTH, HEIGHT);

	renderer = new Renderer();
	sceneOff = new Scene();
	scene = new Scene();

	//pass 01
	auto roomGeo = Geometry::createBox(20, true);
	auto roomMat = new PhongPointShadowMaterial();
	roomMat->mDiffuse = new Texture("assets/textures/wall.jpg", 0, GL_SRGB_ALPHA);;
	roomMat->mShiness = 32;

	auto roomMesh = new Mesh(roomGeo, roomMat);
	sceneOff->addChild(roomMesh);

	//box 们
	auto boxGeo = Geometry::createBox(2);
	auto boxMat = new PhongPointShadowMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/box.png", 0, GL_SRGB_ALPHA);;
	boxMat->mShiness = 32; 

	int lineSize = 6;
	for (int i = 0; i < lineSize; i++) {
		auto boxMesh = new Mesh(boxGeo, boxMat);
		boxMesh->setPosition(glm::vec3(3.0f, 0.0f, i * 2.0f));
		boxMesh->setScale(glm::vec3(1.0, 2.0, 1.0));
		sceneOff->addChild(boxMesh);
	}

	for (int i = 0; i < lineSize; i++) {
		auto boxMesh = new Mesh(boxGeo, boxMat);
		boxMesh->setPosition(glm::vec3(-3.0f, 0.0f, i * 2.0f));
		boxMesh->setScale(glm::vec3(1.0, 2.0, 1.0));
		sceneOff->addChild(boxMesh);
	}

	for (int i = 0; i < lineSize; i++) {
		auto boxMesh = new Mesh(boxGeo, boxMat);
		boxMesh->setPosition(glm::vec3(0.0f, 2.0f, i * 2.0f));
		boxMesh->setScale(glm::vec3(2.0, 1.0, 1.0));
		sceneOff->addChild(boxMesh);
	}

	for (int i = 0; i < lineSize; i++) {
		auto boxMesh = new Mesh(boxGeo, boxMat);
		boxMesh->setPosition(glm::vec3(0.0f, -2.0f, i * 2.0f));
		boxMesh->setScale(glm::vec3(2.0, 1.0, 1.0));
		sceneOff->addChild(boxMesh);
	}

	//pass 02 postProcessPass:后处理pass
	auto sgeo = Geometry::createScreenPlane();
	screenMat = new ScreenMaterial();
	screenMat->mScreenTexture = fbo->mColorAttachment;
	auto smesh = new Mesh(sgeo, screenMat);
	scene->addChild(smesh);


	pointLight = new PointLight();
	pointLight->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	pointLight->mSpecularIntensity = 1.0f;
	pointLight->mK2 = 0.0017f;
	pointLight->mK1 = 0.07f;
	pointLight->mKc = 1.0f;
	pointLight->mColor = glm::vec3(100, 120, 150);
	 
	ambLight = new AmbientLight();
	ambLight->mColor = glm::vec3(0.1f);

}


void prepareCamera() {
	float size = 10.0f;
	//camera = new OrthographicCamera(-size, size, size, -size, size, -size);
	camera = new PerspectiveCamera(
		60.0f,
		(float)glApp->getWidth() / (float)glApp->getHeight(),
		0.1f,
		1000.0f
	);

	cameraControl = new GameCameraControl();
	cameraControl->setCamera(camera);
	cameraControl->setSensitivity(0.4f);
	cameraControl->setSpeed(0.1f);
}



void initIMGUI() {
	ImGui::CreateContext();//创建imgui上下文
	ImGui::StyleColorsDark(); // 选择一个主题

	// 设置ImGui与GLFW和OpenGL的绑定
	ImGui_ImplGlfw_InitForOpenGL(glApp->getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void renderIMGUI() {
	//1 开启当前的IMGUI渲染
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//2 决定当前的GUI上面有哪些控件，从上到下
	ImGui::Begin("MaterialEditor");
	ImGui::SliderFloat("bias:", &pointLight->mShadow->mBias, 0.0f, 0.01f, "%.4f");
	ImGui::SliderFloat("tightness:", &pointLight->mShadow->mDiskTightness, 0.0f, 5.0f, "%.3f");
	ImGui::SliderFloat("pcfRadius:", &pointLight->mShadow->mPcfRadius, 0.0f, 1.0f, "%.4f");
	ImGui::SliderFloat("exposure:", &screenMat->mExposure, 0.0f, 10.0f);

	ImGui::End();

	//3 执行UI渲染
	ImGui::Render();
	//获取当前窗体的宽高
	int display_w, display_h;
	glfwGetFramebufferSize(glApp->getWindow(), &display_w, &display_h);
	//重置视口大小
	glViewport(0, 0, display_w, display_h);

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


int main() {
	if (!glApp->init(WIDTH, HEIGHT)) {
		return -1;
	}

	glApp->setResizeCallback(OnResize);
	glApp->setKeyBoardCallback(OnKey);
	glApp->setMouseCallback(OnMouse);
	glApp->setCursorCallback(OnCursor);
	glApp->setScrollCallback(OnScroll);

	//设置opengl视口以及清理颜色
	GL_CALL(glViewport(0, 0, WIDTH, HEIGHT));
	GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

	prepareCamera();

	prepare();

	initIMGUI();

	while (glApp->update()) {
		cameraControl->update();

		renderer->setClearColor(clearColor);
		renderer->render(sceneOff, camera, pointLight, ambLight, fbo->mFBO);
		renderer->render(scene, camera, pointLight, ambLight);

		renderIMGUI();
	}

	glApp->destroy();

	return 0;
}