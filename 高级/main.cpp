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

Mesh* upPlane = nullptr;

int WIDTH = 2560;
int HEIGHT = 1440;

//灯光们
DirectionalLight* dirLight = nullptr;

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
	fbo = new Framebuffer(WIDTH, HEIGHT);

	renderer = new Renderer();
	sceneOff = new Scene();
	scene = new Scene();

	//pass 01
	auto groundGeo = Geometry::createPlane(20, 500);
	auto mat = new PhongCSMShadowMaterial();
	mat->mDiffuse = new Texture("assets/textures/wall.jpg", 0, GL_SRGB_ALPHA);
	mat->mShiness = 32;

	auto groundMesh = new Mesh(groundGeo, mat);
	groundMesh->setPosition(glm::vec3(0.0, 0.0, 0.0f));
	groundMesh->rotateX(-90.0f);
	sceneOff->addChild(groundMesh);

	for (int i = 0; i < 50; i++) {
		auto geo = Geometry::createBox(1.5);
		auto mesh = new Mesh(geo, mat);
		mesh->setPosition(glm::vec3(i % 3 * 3, 0.0, -i / 3 * 3));
		sceneOff->addChild(mesh);
	}

	//pass 02 postProcessPass:后处理pass
	auto sgeo = Geometry::createScreenPlane();
	auto smat = new ScreenMaterial();
	smat->mScreenTexture = fbo->mColorAttachment;
	auto smesh = new Mesh(sgeo, smat);
	scene->addChild(smesh);


	dirLight = new DirectionalLight();
	dirLight->setPosition(glm::vec3(10.0f, 10.0f, 0.0f));
	dirLight->rotateX(-45.0f);
	dirLight->rotateY(45.0f);
	dirLight->mSpecularIntensity = 0.5f;

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
	ImGui::SliderFloat("bias:", &dirLight->mShadow->mBias, 0.0f, 0.01f, "%.4f");
	ImGui::SliderFloat("tightness:", &dirLight->mShadow->mDiskTightness, 0.0f, 5.0f, "%.3f");
	ImGui::SliderFloat("pcfRadius:", &dirLight->mShadow->mPcfRadius, 0.0f, 1.0f, "%.4f");

	/*int width = dirLight->mShadow->mRenderTarget->mWidth;
	int height = dirLight->mShadow->mRenderTarget->mHeight;
	if (
		ImGui::SliderInt("FBO width:", &width, 1, 4096) ||
		ImGui::SliderInt("FBO height:", &height, 1, 4096)
		) {
		dirLight->mShadow->setRenderTargetSize(width, height);
	}*/
	ImGui::SliderFloat("Light Size:", &dirLight->mShadow->mLightSize, 0.0f, 10.0f);

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

bool goUp = true;
void transform() {
	auto pos = upPlane->getPosition();
	if (goUp) {
		pos.y += 0.05;
		upPlane->setPosition(pos);
		if (pos.y > 10) {
			goUp = false;
		}
	}
	else {
		pos.y -= 0.05;
		upPlane->setPosition(pos);
		if (pos.y < 0.2) {
			goUp = true;
		}
	}
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
		//transform();

		renderer->setClearColor(clearColor);
		renderer->render(sceneOff, camera, dirLight, ambLight, fbo->mFBO);
		renderer->render(scene, camera, dirLight, ambLight);

		renderIMGUI();
	}

	glApp->destroy();

	return 0;
}