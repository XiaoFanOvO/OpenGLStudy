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


Renderer* renderer = nullptr;
Scene* sceneOffscreen = nullptr;//离屏渲染场景
Scene* sceneInscreen = nullptr;//贴屏渲染场景
Scene* scene = nullptr; //通用测试场景

Framebuffer* framebuffer = nullptr;

//这里先写死,后面需要单独进行管理
int WIDTH = 1600;
int HEIGHT = 1200;

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

//void prepareFBO() {
//	//1 生成FBO对象并且绑定
//	glGenFramebuffers(1, &fbo);
//	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//	//2 生成颜色附件,并且加入FBO
//	colorAttachment = new Texture(WIDTH, HEIGHT, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment->getTexture(), 0);
//	//3 生成depth Stencil附件,加入FBO
//	unsigned int depthStencil;
//	glGenTextures(1, &depthStencil);
//	glBindTexture(GL_TEXTURE_2D, depthStencil);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);
//	//检查当前构建的fbo是否完整
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//	{
//		std::cout << "Error: FrameBuffer is not complete" << std::endl;
//	}
//	glBindFramebuffer(GL_FRAMEBUFFER, 0); //解绑FBO
//}

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

void setModelBlend(Object* obj, bool blend, float opacity) {
	if (obj->getType() == ObjectType::Mesh)
	{
		Mesh* mesh = (Mesh*)obj;
		Material* mat = mesh->mMaterial;
		mat->mBlend = blend;
		mat->mOpacity = opacity;
		mat->mDepthWrite = false;
	}
	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		setModelBlend(children[i], blend, opacity);
	}
}


void prepare() {
	renderer = new Renderer();
	scene = new Scene();
	
	//std::vector<std::string> paths = {
	//	"assets/textures/skybox/right.jpg",
	//	"assets/textures/skybox/left.jpg",
	//	"assets/textures/skybox/top.jpg",
	//	"assets/textures/skybox/bottom.jpg",
	//	"assets/textures/skybox/back.jpg",
	//	"assets/textures/skybox/front.jpg",
	//};

	////1 先绘制天空盒，就需要关闭它的深度写入
	//auto boxGeo = Geometry::createBox(1.0f);
	//auto boxMat = new CubeMaterial();
	//boxMat->mDiffuse = new Texture(paths, 0);
	//boxMat->mDepthWrite = false;//先绘制的话挡不住后面
	//auto boxMesh = new Mesh(boxGeo, boxMat);
	//scene->addChild(boxMesh);
	//auto sphereGeo = Geometry::createSphere(4.0f);
	//auto sphereMat = new PhongMaterial();
	//sphereMat->mDiffuse = new Texture("assets/textures/earth.png", 0);
	//auto sphereMesh = new Mesh(sphereGeo, sphereMat);
	//scene->addChild(sphereMesh);

	////2 先绘制球体再绘制天空盒，球体由于是最后一个绘制，写不写入深度都无所谓了
	//auto sphereGeo = Geometry::createSphere(4.0f);
	//auto sphereMat = new PhongMaterial();
	//sphereMat->mDiffuse = new Texture("assets/textures/earth.png", 0);
	//auto sphereMesh = new Mesh(sphereGeo, sphereMat);
	//scene->addChild(sphereMesh);
	//auto boxGeo = Geometry::createBox(1.0f);
	//auto boxMat = new CubeMaterial();
	//boxMat->mDiffuse = new Texture(paths, 0);
	//boxMat->mDepthWrite = false;
	//auto boxMesh = new Mesh(boxGeo, boxMat);
	//scene->addChild(boxMesh);

	//3 最好的办法就是天空盒的深度永远是1 永远通不过深度检测 这样就和绘制顺序没关系了
	//gl_Position = gl_Position.xyww;


	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new CubeMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/bk.jpg", 0);
	auto boxMesh = new Mesh(boxGeo, boxMat);
	scene->addChild(boxMesh);

	auto sphereGeo = Geometry::createSphere(4.0f);
	auto sphereMat = new PhongInstanceMaterial();
	sphereMat->mDiffuse = new Texture("assets/textures/earth.png", 3);
	
	auto sphereMesh = new InstancedMesh(sphereGeo, sphereMat,3);
	glm::mat4 transform0 = glm::mat4(1.0f);
	glm::mat4 transform1 = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 0.0f));
	glm::mat4 transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 0.0f));
	sphereMesh->mInstanceMatrices[0] = transform0;
	sphereMesh->mInstanceMatrices[1] = transform1;
	sphereMesh->mInstanceMatrices[2] = transform2;
	sphereMesh->updateMatrices();
	scene->addChild(sphereMesh);



	

	
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
	//GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
	GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

	prepareCamera();
	//注意顺序问题!!!! 否则colorattachment为nullptr
	//prepareFBO();
	prepare();
	initIMGUI();

	while (glApp->update()) {
		cameraControl->update();
		renderer->setClearColor(clearColor);
		//pass01 将box渲染到colorAttachmengt上,也就是新的fbo上
		//renderer->render(sceneOffscreen, camera, dirLight, ambLight, framebuffer->mFBO);//这里采用我们自己的fbo

		//pass02 将colorAttachment作为纹理,绘制到整个屏幕上
		renderer->render(scene, camera, dirLight, ambLight);//这里是默认的fbo
		renderIMGUI();
	}

	glApp->destroy();

	return 0;
}