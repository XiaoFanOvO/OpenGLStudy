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
#include "glframework/material/screenMaterial.h"
#include "glframework/material/cubeMaterial.h"
#include "glframework/material/phongEnvMaterial.h"
#include "glframework/material/phongInstanceMaterial.h"
#include "glframework/material/grassInstanceMaterial.h"
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
#include "application/assimpInstanceLoader.h"

#include "glframework/framebuffer/framebuffer.h"


Renderer* renderer = nullptr;
Scene* sceneOffscreen = nullptr;//������Ⱦ����
Scene* sceneInscreen = nullptr;//������Ⱦ����
Scene* scene = nullptr; //ͨ�ò��Գ���

Framebuffer* framebuffer = nullptr;

//������д��,������Ҫ�������й���
int WIDTH = 1600;
int HEIGHT = 1200;

GrassInstanceMaterial* grassMaterial = nullptr;//�������Ҫͨ��imguiȥ����,���Էŵ�ȫ��

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

//void prepareFBO() {
//	//1 ����FBO�����Ұ�
//	glGenFramebuffers(1, &fbo);
//	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//	//2 ������ɫ����,���Ҽ���FBO
//	colorAttachment = new Texture(WIDTH, HEIGHT, 0);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment->getTexture(), 0);
//	//3 ����depth Stencil����,����FBO
//	unsigned int depthStencil;
//	glGenTextures(1, &depthStencil);
//	glBindTexture(GL_TEXTURE_2D, depthStencil);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);
//	//��鵱ǰ������fbo�Ƿ�����
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//	{
//		std::cout << "Error: FrameBuffer is not complete" << std::endl;
//	}
//	glBindFramebuffer(GL_FRAMEBUFFER, 0); //���FBO
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

void setInstanceMatrix(Object* obj, int index, glm::mat4 matrix) {
	if (obj->getType() == ObjectType::InstancedMesh)
	{
		InstancedMesh* im = (InstancedMesh*)obj;
		im->mInstanceMatrices[index] = matrix;
	}
	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		setInstanceMatrix(children[i], index, matrix);
	}
}

void updateInstanceMatrix(Object* obj) {
	if (obj->getType() == ObjectType::InstancedMesh)
	{
		InstancedMesh* im = (InstancedMesh*)obj;
		im->updateMatrices();
	}
	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		updateInstanceMatrix(children[i]);
	}
} 

void setInstanceMaterial(Object* obj, Material* material) {
	if (obj->getType() == ObjectType::InstancedMesh)
	{
		InstancedMesh* im = (InstancedMesh*)obj;
		im->mMaterial = material;
	}
	auto children = obj->getChildren();
	for (int i = 0; i < children.size(); i++)
	{
		setInstanceMaterial(children[i], material);
	}
}

void prepare() {
	renderer = new Renderer();
	scene = new Scene();
	

	auto boxGeo = Geometry::createBox(1.0f);
	auto boxMat = new CubeMaterial();
	boxMat->mDiffuse = new Texture("assets/textures/bk.jpg", 0);
	auto boxMesh = new Mesh(boxGeo, boxMat);
	scene->addChild(boxMesh);

	int rNum = 50;
	int cNum = 50;

	auto grassModel = AssimpInstanceLoader::load("assets/fbx/grassNew.obj", rNum * cNum);
	glm::mat4 translate;//ƽ��
	glm::mat4 rotate;//��ת
	glm::mat4 transform;//�ϲ�����

	srand(glfwGetTime());//�����������
	for (int r = 0; r < rNum; r++)
	{
		for (int c = 0; c < cNum; c++)
		{
			translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.2 * r, 0.0f, 0.2 * c));
			rotate = glm::rotate(glm::radians((float)(rand() % 90)), glm::vec3(0.0, 1.0, 0.0));
			transform = translate * rotate;
			setInstanceMatrix(grassModel, r * cNum + c, transform);
		}
	}
	updateInstanceMatrix(grassModel);

	/*auto transform0 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	auto transform1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	setInstanceMatrix(grassModel, 0, transform0);
	setInstanceMatrix(grassModel, 1, transform1);
	updateInstanceMatrix(grassModel);*/

	grassMaterial = new GrassInstanceMaterial();
	grassMaterial->mDiffuse = new Texture("assets/textures/GRASS.png", 0);
	grassMaterial->mOpacityMask = new Texture("assets/textures/grassMask.png", 1);
	grassMaterial->mCloudMask = new Texture("assets/textures/CLOUD.PNG", 2);
	grassMaterial->mBlend = true;
	grassMaterial->mDepthWrite = false;
	setInstanceMaterial(grassModel, grassMaterial);
	scene->addChild(grassModel);


	auto house = AssimpLoader::load("assets/fbx/house.fbx");
	house->setScale(glm::vec3(0.5f));
	house->setPosition(glm::vec3(rNum * 0.2f / 2.0f, 0.4f, cNum * 0.2f / 2.0f));
	scene->addChild(house);



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
	ImGui::Begin("GrassMaterialEditor");
	ImGui::Text("GrassColor");
	ImGui::SliderFloat("UVScale", &grassMaterial->mUVScale, 0.0f, 100.0f);
	ImGui::InputFloat("Brightness", &grassMaterial->mBrightness);
	ImGui::Text("Wind");
	ImGui::InputFloat("WindScale", &grassMaterial->mWindScale);
	ImGui::InputFloat("PhaseScale", &grassMaterial->mPhaseScale);
	ImGui::ColorEdit3("WindDirection", (float*)&grassMaterial->mWindDirection);
	ImGui::Text("Cloud");
	ImGui::ColorEdit3("CloudWhiteColor", (float*)&grassMaterial->mCloudWhiteColor);
	ImGui::ColorEdit3("CloudBlackColor", (float*)&grassMaterial->mCloudBlackColor);
	ImGui::SliderFloat("CloudUVScale", &grassMaterial->mCloudUVScale, 0.0f, 100.0f);
	ImGui::InputFloat("CloudSpeed", &grassMaterial->mCloudSpeed);
	ImGui::SliderFloat("CloudLerp", &grassMaterial->mCloudLerp, 0.0f, 1.0f);
	ImGui::Text("Light");
	ImGui::InputFloat("Intensity", &dirLight->mIntensity);
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
	if (!glApp->init(WIDTH, HEIGHT)) {
		return -1;
	}

	glApp->setResizeCallback(OnResize);
	glApp->setKeyBoardCallback(OnKey);
	glApp->setMouseCallback(OnMouse);
	glApp->setCursorCallback(OnCursor);
	glApp->setScrollCallback(OnScroll);

	//����opengl�ӿ��Լ�������ɫ
	GL_CALL(glViewport(0, 0, WIDTH, HEIGHT));
	//GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
	GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

	prepareCamera();
	//ע��˳������!!!! ����colorattachmentΪnullptr
	//prepareFBO();
	prepare();
	initIMGUI();

	while (glApp->update()) {
		cameraControl->update();
		renderer->setClearColor(clearColor);
		//pass01 ��box��Ⱦ��colorAttachmengt��,Ҳ�����µ�fbo��
		//renderer->render(sceneOffscreen, camera, dirLight, ambLight, framebuffer->mFBO);//������������Լ���fbo

		//pass02 ��colorAttachment��Ϊ����,���Ƶ�������Ļ��
		renderer->render(scene, camera, dirLight, ambLight);//������Ĭ�ϵ�fbo
		renderIMGUI();
	}

	glApp->destroy();

	return 0;
}