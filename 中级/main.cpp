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
#include "glframework/mesh.h"
#include "glframework/renderer/renderer.h"
#include "glframework/light/pointLight.h"
#include "glframework/light/spotLight.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glframework/scene.h"
#include "application/assimpLoader.h"


Renderer* renderer = nullptr;
Scene* scene = nullptr;

// 灯光
std::vector<PointLight*> pointLights{};
SpotLight* spotLight = nullptr;
DirectionalLight* dirLight = nullptr;

//环境光
AmbientLight* ambLight = nullptr;

Camera* camera = nullptr;
CameraControl* cameraControl = nullptr;

glm::vec3 clearColor{};

#pragma region 事件回调函数
void OnResize(int width, int height) {
	GL_CALL(glViewport(0, 0, width, height));
	std::cout << "OnResize" << std::endl;
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
#pragma endregion

#pragma region 老版本的调用方式
//void prepareVAO() {
//	//geometry = Geometry::createBox(3.0f);
//	geometry = Geometry::createSphere(2.0f);
//}
//
//void prepareShader() {
//	shader = new Shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
//}
//
//void prepareTexture() {
//	texture = new Texture("assets/textures/goku.jpg", 0);
//}
//
//void prepareState() {
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//}
//
//void doTransform() {
//	transform = glm::rotate(transform, 0.03f, glm::vec3(0.0f, 1.0f, 1.0f));
//}

void render() {
	////执行opengl画布清理操作
	//GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	////绑定当前的program
	//shader->begin();
	//shader->setInt("sampler", 0);
	//shader->setMatrix4x4("modelMatrix", transform);
	//shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
	//shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
	//
	////光源参数的uniform更新 
	//shader->setVector3("lightDirection", lightDirection);
	//shader->setVector3("lightColor", lightColor);
	//shader->setVector3("cameraPosition", camera->mPosition);
	//shader->setFloat("specularIntensity", specularIntensity);
	//shader->setVector3("ambientColor", ambientColor);
 //
	////绑定当前的vao
	//GL_CALL(glBindVertexArray(geometry->getVao()));

	////发出绘制指令
	//GL_CALL(glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0));
	//GL_CALL(glBindVertexArray(0));

	//shader->end();
}
#pragma endregion

void prepareCamera() {
	float size = 10.0f;
	//camera = new OrthographicCamera(-size, size, size, -size, size, -size);
	camera = new PerspectiveCamera(
		60.0f,
		(float)glApp->getWidth() / (float)glApp->getHeight(),
		0.1f,
		50.0f
	);

	cameraControl = new GameCameraControl();
	cameraControl->setCamera(camera);
	cameraControl->setSensitivity(0.4f);
}

 
void prepare() {
	renderer = new Renderer();
	scene = new Scene();

	////------------------A方块的实体与边界-----------------
	////1 创建一个普通方块
	//auto geometryA = Geometry::createBox(4.0f);
	//auto materialA = new PhongMaterial();
	//materialA->mDiffuse = new Texture("assets/textures/goku.jpg", 0);
	//materialA->mStencilTest = true;
	////模板测试后的结果写入
	//materialA->mSFail = GL_KEEP;
	//materialA->mZFail = GL_KEEP;
	//materialA->mZPass = GL_REPLACE;
	////控制写入
	//materialA->mStencilMask = 0xff;
	////模板测试规则
	//materialA->mStencilFunc = GL_ALWAYS;
	//materialA->mStencilRef = 1;
	//materialA->mStencilFuncMask = 0xff;

	//auto meshA = new Mesh(geometryA, materialA);
	//scene->addChild(meshA);


	////2 创建一个勾边方块
	//auto materialABound = new WhiteMaterial();
	//materialABound->mDepthTest = false;
	//materialABound->mStencilTest = true;
	////模板测试后的结果写入
	//materialABound->mSFail = GL_KEEP;
	//materialABound->mZFail = GL_KEEP;
	//materialABound->mZPass = GL_KEEP;
	////控制写入
	//materialABound->mStencilMask = 0x00;
	////模板测试规则
	//materialABound->mStencilFunc = GL_NOTEQUAL;
	//materialABound->mStencilRef = 1;
	//materialABound->mStencilFuncMask = 0xff;
	//auto meshABound = new Mesh(geometryA, materialABound);
	//meshABound->setPosition(meshA->getPosition());
	//meshABound->setScale(glm::vec3(1.2f));
	//scene->addChild(meshABound);


	////------------------B方块的实体与边界-----------------
	////1 创建一个普通方块
	//auto geometryB = Geometry::createBox(4.0f);
	//auto materialB = new PhongMaterial();
	//materialB->mDiffuse = new Texture("assets/textures/wall.jpg", 0);
	//materialB->mStencilTest = true;
	////模板测试后的结果写入
	//materialB->mSFail = GL_KEEP;
	//materialB->mZFail = GL_KEEP;
	//materialB->mZPass = GL_REPLACE;
	////控制写入
	//materialB->mStencilMask = 0xff;
	////模板测试规则
	//materialB->mStencilFunc = GL_ALWAYS;
	//materialB->mStencilRef = 1;
	//materialB->mStencilFuncMask = 0xff;

	//auto meshB = new Mesh(geometryB, materialB);
	//meshB->setPosition(glm::vec3(3.0f, 1.0f, 1.0f));
	//scene->addChild(meshB);


	////2 创建一个勾边方块
	//auto materialBBound = new WhiteMaterial();
	//materialBBound->mDepthTest = false;
	//materialBBound->mStencilTest = true;
	////模板测试后的结果写入
	//materialBBound->mSFail = GL_KEEP;
	//materialBBound->mZFail = GL_KEEP;
	//materialBBound->mZPass = GL_KEEP;
	////控制写入
	//materialBBound->mStencilMask = 0x00;
	////模板测试规则
	//materialBBound->mStencilFunc = GL_NOTEQUAL;
	//materialBBound->mStencilRef = 1;
	//materialBBound->mStencilFuncMask = 0xff;
	//auto meshBBound = new Mesh(geometryA, materialBBound);
	//meshBBound->setPosition(meshB->getPosition());
	//meshBBound->setScale(glm::vec3(1.2f));
	//scene->addChild(meshBBound);


	/*auto boxGeo = Geometry::createBox(4.0f);
	auto boxMat = new PhongMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/box.png", 0);
	auto boxMesh = new Mesh(boxGeo, boxMat);
	scene->addChild(boxMesh);

	auto planeGeo = Geometry::createPlane(6.0f, 6.0f);
	auto planeMat = new PhongMaterial();
	planeMat->mDiffuse = new Texture("assets/textures/window.png", 0);
	auto planeMesh = new Mesh(planeGeo, planeMat);
	planeMesh->setPosition(glm::vec3(0.0f, 0.0f, 4.0f));
	scene->addChild(planeMesh);*/


	auto boxGeo = Geometry::createBox(5.0f);
	auto boxMat = new WhiteMaterial();
	auto boxMesh = new Mesh(boxGeo, boxMat);
	boxMesh->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
	scene->addChild(boxMesh);


	auto model = AssimpLoader::load("assets/fbx/bag/backpack.obj");
	scene->addChild(model);


	//方向光
	dirLight = new DirectionalLight();
	dirLight->mDirection = glm::vec3(-1.0f);
	dirLight->mSpecularIntensity = 0.1f;
	
	//环境光
	ambLight = new AmbientLight();
	ambLight->mColor = glm::vec3(0.1f);
}

void initIMGUI() {
	ImGui::CreateContext(); //创建imgui上下文
	ImGui::StyleColorsDark(); //选择一个主题

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
	ImGui::Begin("Hello, world!");
	ImGui::Text("ChangeColor Demo");
	ImGui::Button("Test Button", ImVec2(40, 20));
	ImGui::ColorEdit3("Clear Color", (float*)&clearColor);
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
	if (!glApp->init(1600, 1200)) {
		return -1;
	}

	glApp->setResizeCallback(OnResize);
	glApp->setKeyBoardCallback(OnKey);
	glApp->setMouseCallback(OnMouse);
	glApp->setCursorCallback(OnCursor);
	glApp->setScrollCallback(OnScroll);

	//设置opengl视口以及清理颜色
	GL_CALL(glViewport(0, 0, 1600, 1200));
	//GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
	GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

	prepareCamera();
	prepare();
	initIMGUI();

	while (glApp->update()) {
		cameraControl->update();
		renderer->setClearColor(clearColor);
		renderer->render(scene, camera, dirLight, ambLight);
		renderIMGUI();
	}

	glApp->destroy();

	return 0;
}