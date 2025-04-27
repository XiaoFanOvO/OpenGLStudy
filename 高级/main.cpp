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

int WIDTH = 2560;
int HEIGHT = 1440;

GrassInstanceMaterial* grassMaterial = nullptr;

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
	//glEnable(GL_FRAMEBUFFER_SRGB);

	fbo = new Framebuffer(WIDTH, HEIGHT);

	renderer = new Renderer();
	sceneOff = new Scene();
	scene = new Scene();

	//pass 01
	float halfW = 2.5f, halfH = 3.5f;
	std::vector<float> positions = {
		-halfW, -halfH, 0.0f,
		halfW, -halfH, 0.0f,
		halfW, halfH, 0.0f,
		-halfW, halfH, 0.0f,
	};

	std::vector<float> uvs = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	std::vector<float> normals = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,
		2, 3, 0
	};

	//计算tangents
	std::vector<float> tangents = {};
	//位置
	glm::vec3 pos1(positions[0], positions[1], positions[2]);
	glm::vec3 pos2(positions[3], positions[4], positions[5]);
	glm::vec3 pos3(positions[6], positions[7], positions[8]);
	//uv
	glm::vec2 uv1(uvs[0], uvs[1]);
	glm::vec2 uv2(uvs[2], uvs[3]);
	glm::vec2 uv3(uvs[4], uvs[5]);

	glm::vec3 e1 = pos2 - pos1;
	glm::vec3 e2 = pos3 - pos2;

	glm::vec2 dUV1 = uv2 - uv1;
	glm::vec2 dUV2 = uv3 - uv2;

	float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

	glm::vec3 tangent;
	tangent.x = f * (dUV2.y * e1.x - dUV1.y * e2.x);
	tangent.y = f * (dUV2.y * e1.y - dUV1.y * e2.y);
	tangent.z = f * (dUV2.y * e1.z - dUV1.y * e2.z);

	for (int i = 0; i < 4; i++) {
		tangents.push_back(tangent.x);
		tangents.push_back(tangent.y);
		tangents.push_back(tangent.z);
	}

	auto planeGeo = new Geometry(positions, normals, uvs, indices, tangents);
	auto planeMat = new PhongNormalMaterial();
	planeMat->mDiffuse = new Texture("assets/textures/normal/brickwall.jpg", 0, GL_SRGB_ALPHA);
	planeMat->mNormalMap = new Texture("assets/textures/normal/normal_map.png", 1);
	planeMat->mShiness = 32;
	auto mesh = new Mesh(planeGeo, planeMat);
	mesh->rotateX(-90.0f);
	sceneOff->addChild(mesh);


	//pass 02
	auto sgeo = Geometry::createScreenPlane();
	auto smat = new ScreenMaterial();
	smat->mScreenTexture = fbo->mColorAttachment;
	auto smesh = new Mesh(sgeo, smat);
	scene->addChild(smesh);

	
	dirLight = new DirectionalLight();
	//dirLight->mDirection = glm::vec3(0.0f, -0.4f, -1.0f);
	dirLight->mDirection = glm::vec3(0.0f, -1.0f,0.0f);
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
	ImGui::Begin("GrassMaterialEditor");
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
		
		renderer->render(sceneOff, camera,dirLight, ambLight, fbo->mFBO);
		renderer->render(scene, camera, dirLight, ambLight);

		renderIMGUI();
	}

	glApp->destroy();

	return 0;
}