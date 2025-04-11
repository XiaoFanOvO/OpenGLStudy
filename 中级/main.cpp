#include <iostream>

#include "glframework/core.h"
#include "glframework/shader.h"
#include <string>
#include <assert.h>//����
#include "wrapper/checkError.h"
#include "application/Application.h"
#include "glframework/texture.h"

//�������+������
#include "application/camera/perspectiveCamera.h"
#include "application/camera/orthographicCamera.h"
#include "application/camera/trackBallCameraControl.h"
#include "application/camera/GameCameraControl.h"

#include "glframework/geometry.h"
#include "glframework/material/phongMaterial.h"
#include "glframework/material/whiteMaterial.h"
#include "glframework/material/depthMaterial.h"
#include "glframework/material/opacityMaskMaterial.h"
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

// �ƹ�
std::vector<PointLight*> pointLights{};
SpotLight* spotLight = nullptr;
DirectionalLight* dirLight = nullptr;

//������
AmbientLight* ambLight = nullptr;

Camera* camera = nullptr;
CameraControl* cameraControl = nullptr;

glm::vec3 clearColor{};

#pragma region �¼��ص�����
void OnResize(int width, int height) {
	GL_CALL(glViewport(0, 0, width, height));
	std::cout << "OnResize" << std::endl;
}

void OnKey(int key, int action, int mods) {
	cameraControl->onKey(key, action, mods);
}

//��갴��/̧��
void OnMouse(int button, int action, int mods) {
	double x, y;
	glApp->getCursorPosition(&x, &y);
	cameraControl->onMouse(button, action, x, y);
}

//����ƶ�
void OnCursor(double xpos, double ypos) {
	cameraControl->onCursor(xpos, ypos);
}

//������
void OnScroll(double offset) {
	cameraControl->onScroll(offset);
}
#pragma endregion

#pragma region �ϰ汾�ĵ��÷�ʽ
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
	////ִ��opengl�����������
	//GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	////�󶨵�ǰ��program
	//shader->begin();
	//shader->setInt("sampler", 0);
	//shader->setMatrix4x4("modelMatrix", transform);
	//shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
	//shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
	//
	////��Դ������uniform���� 
	//shader->setVector3("lightDirection", lightDirection);
	//shader->setVector3("lightColor", lightColor);
	//shader->setVector3("cameraPosition", camera->mPosition);
	//shader->setFloat("specularIntensity", specularIntensity);
	//shader->setVector3("ambientColor", ambientColor);
 //
	////�󶨵�ǰ��vao
	//GL_CALL(glBindVertexArray(geometry->getVao()));

	////��������ָ��
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


	auto grassModel = AssimpLoader::load("assets/fbx/grass.fbx");
	grassModel->setScale(glm::vec3(0.02f));
	scene->addChild(grassModel);

	auto grassMat = new OpacityMaskMaterial();
	grassMat->mDiffuse = new Texture("assets/textures/grass.jpg", 0);
	grassMat->mOpacityMask = new Texture("assets/textures/grassMask.png", 1);
	grassMat->mBlend = true;
	grassMat->mDepthWrite = false;
	grassMat->mFaceCulling = true;
	grassMat->mFrontFace = GL_CCW;
	grassMat->mCullFace = GL_BACK;
	renderer->mGlobalMaterial = grassMat;
	 
	//�����
	dirLight = new DirectionalLight();
	dirLight->mDirection = glm::vec3(-1.0f);
	dirLight->mSpecularIntensity = 0.1f;
	
	//������
	ambLight = new AmbientLight();
	ambLight->mColor = glm::vec3(0.1f);
}

void initIMGUI() {
	ImGui::CreateContext(); //����imgui������
	ImGui::StyleColorsDark(); //ѡ��һ������

	// ����ImGui��GLFW��OpenGL�İ�
	ImGui_ImplGlfw_InitForOpenGL(glApp->getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void renderIMGUI() {
	//1 ������ǰ��IMGUI��Ⱦ
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//2 ������ǰ��GUI��������Щ�ؼ������ϵ���
	ImGui::Begin("Hello, world!");
	ImGui::Text("ChangeColor Demo");
	ImGui::Button("Test Button", ImVec2(40, 20));
	ImGui::ColorEdit3("Clear Color", (float*)&clearColor);
	ImGui::End();
	
	//3 ִ��UI��Ⱦ
	ImGui::Render();
	//��ȡ��ǰ����Ŀ��
	int display_w, display_h;
	glfwGetFramebufferSize(glApp->getWindow(), &display_w, &display_h);
	//�����ӿڴ�С
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

	//����opengl�ӿ��Լ�������ɫ
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