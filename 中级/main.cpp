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

glm::vec3 lightDirection = glm::vec3(-0.4f, -1.4, -1.9f);
glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

Geometry* geometry = nullptr;
Shader* shader = nullptr;
Texture* texture = nullptr;
glm::mat4 transform(1.0f);

Camera* camera = nullptr;
CameraControl* cameraControl = nullptr;

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
	app->getCursorPosition(&x, &y);
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

void prepareVAO() {
	geometry = Geometry::createBox(3.0f);
}

void prepareShader() {
	shader = new Shader("assets/shaders/vertex.glsl","assets/shaders/fragment.glsl");
}

void prepareTexture() {
	texture = new Texture("assets/textures/goku.jpg", 0);
}

void prepareCamera() {
	float size = 10.0f;
	//camera = new OrthographicCamera(-size, size, size, -size, size, -size);
	camera = new PerspectiveCamera(
		60.0f, 
		(float)app->getWidth() / (float)app->getHeight(),
		0.1f,
		1000.0f
	);

	cameraControl = new GameCameraControl();
	cameraControl->setCamera(camera);
	cameraControl->setSensitivity(0.4f);
}

void prepareState() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void render() {
	//ִ��opengl�����������
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//�󶨵�ǰ��program
	shader->begin();
	shader->setInt("sampler", 0);
	shader->setMatrix4x4("modelMatrix", transform);
	shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
	shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
	
	//��Դ������uniform����
	shader->setVector3("lightDirection", lightDirection);
	shader->setVector3("lightColor", lightColor);
	shader->setVector3("cameraPosition", camera->mPosition);
 
	//�󶨵�ǰ��vao
	GL_CALL(glBindVertexArray(geometry->getVao()));

	//��������ָ��
	GL_CALL(glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0));
	GL_CALL(glBindVertexArray(0));

	shader->end();
}


int main() {
	if (!app->init(1600, 1200)) {
		return -1;
	}

	app->setResizeCallback(OnResize);
	app->setKeyBoardCallback(OnKey);
	app->setMouseCallback(OnMouse);
	app->setCursorCallback(OnCursor);
	app->setScrollCallback(OnScroll);

	//����opengl�ӿ��Լ�������ɫ
	GL_CALL(glViewport(0, 0, 1600, 1200));
	GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));

	prepareShader();
	prepareVAO();
	prepareTexture();
	prepareCamera();
	prepareState();

	while (app->update()) {
		cameraControl->update();
		render();
	}

	app->destroy();

	return 0;
}