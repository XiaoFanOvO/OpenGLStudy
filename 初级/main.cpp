#include <iostream>

#include "glframework/core.h"
#include "glframework/shader.h"
#include <string>
#include <assert.h>//����
#include "wrapper/checkError.h"
#include "application/application.h"
#include "glframework/texture.h"

//�������+������
#include "application/camera/perspectiveCamera.h"
#include "application/camera/trackBallCameraControl.h"
#include "application/camera/orthographicCamera.h"
#include "application/camera/gameCameraControl.h"

#include "glframework/geometry.h"

GLuint vao;
Shader* shader = nullptr;
Texture* texture = nullptr;
Texture* texture2 = nullptr;
Texture* grassTexture = nullptr;
Texture* landTexture = nullptr;
Texture* noiseTexture = nullptr;
glm::mat4 transform(1.0);
glm::mat4 transform2(1.0);
//glm::mat4 viewMatrix(1.0f);
//glm::mat4 orthoMatrix(1.0f);//����ͶӰ����
//glm::mat4 perspectiveMatrix(1.0f);//͸��ͶӰ����

PerspectiveCamera* camera = nullptr;
TrackBallCameraControl* cameraControl = nullptr;
//OrthographicCamera* orthCamera = nullptr;
glm::mat4 viewMatrix(1.0f);
glm::mat4 perspectiveMatrix(1.0f);

Geometry* geometry = nullptr;

//������ʵ��һ����Ӧ�����С�仯�ĺ���
void frameBufferSizeCallBack(GLFWwindow* window, int width, int height) {
	//std::cout << "�������´�С��" << width << " , " << height << std::endl;
	//���ӿڴ�С�ʹ��ڴ�Сһ��ı�
	GL_CALL(glViewport(0, 0, width, height));
}

void OnResize(int width, int height){
	GL_CALL(glViewport(0, 0, width, height));
	std::cout << "OnResize" << std::endl;
}

void OnKey(int key, int action, int mods) {
	//std::cout << "OnKey" << std::endl;
	cameraControl->onKey(key, action, mods);
}

//��갴�»�̧��
void OnMouse(int button, int action, int mods) {
	//std::cout << "OnMouse" << std::endl;
	double x, y;
	app->getCursorPosition(&x, &y);
	cameraControl->onMouse(button, action, x, y);
}

//����ƶ�
void OnCursor(double xpos, double ypos) {
	//std::cout << "OnCursor" << std::endl;
	cameraControl->onCursor(xpos, ypos);
}

//������
void OnScroll(double offset) {
	cameraControl->onScroll(offset);
}

//��ת�任
//Ĭ����ԭ��Ϊ����
void doRotationTransform() {
	//����һ����ת����,����z����ת45�Ƚ�
	//rotate����:����������ת����
	//bug1: rotate�������õ�һ��float���͵ĽǶ�
	//bug2: rotate�������ܵĲ��ǽǶ�(degree)  ���ǻ���(radians)
	//radians����Ҳ��ģ�溯��,�мǴ���float��������
	transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
}

//ƽ�Ʊ任
void doTranslationTransform() {
	transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f));
}

//���ű任
void doScaleTransform() {
	transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 1.0f));
}

void preTransform() {
	//Ŀ���:��ƽ���ٵ�����ת
	//ƽ��ֻ��һ��,���ܷŵ�whiletrue����
	//transform = glm::translate(transform, glm::vec3(0.6f, 0.0f, 0.0f));

	//Ŀ����:����һ����ת,ֻ��תһ��
	//float angle = 45.0f;
	//transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

	//Ŀ����:����һ������
	transform = glm::scale(transform, glm::vec3(0.5f, 1.0f, 1.0f));
}

void doTransform() {
	//Ŀ��һ:��ת��������
	//float angle = 1.0f;
	//transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));//��ת����� ��Ϊ�ǻ�����һ�α任

	//Ŀ���:��ƽ���ٵ�����ת
	//��תÿ֡����
	//float angle = 1.0f;
	//transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

	//Ŀ����:����ת�ٵ���ƽ��
	//1 preTransform��������һ����ת
	//2 ÿһ֡ƽ��
	//transform = glm::translate(transform, glm::vec3(0.01f, 0.0f, 0.0f));

	//Ŀ����:����һ�������ٵ���ƽ��
	transform = glm::translate(transform, glm::vec3(0.01f, 0.0f, 0.0f));
}

//��ת����
float angle = 0.0f;
void doRotation() {
	angle += 2.0f;
	//ÿһ֡�������¹���һ����ת����(����һ����ת����û�й�ϵ)
	transform = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
}

void prepareSingleBuffer() {
	//1.׼��positions colors����
	float positions[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};

	float colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	//2.ʹ��������������vbo  posVBO  colorVBO
	GLuint posVbo, colorVbo;
	GL_CALL(glGenBuffers(1, &posVbo));
	GL_CALL(glGenBuffers(1, &colorVbo));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posVbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, colorVbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));

	//3.����vao���Ұ�
	GLuint vao = 0;
	GL_CALL(glGenVertexArrays(1, &vao));
	GL_CALL(glBindVertexArray(vao));

	//4.�ֱ�λ��/��ɫ���Ե�������Ϣ����vao����
	//4.1 ����λ������
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posVbo));//ֻ�а���posVbo ��������������Ż����vbo���
	GL_CALL(glEnableVertexAttribArray(0)); // ��λ�����Էŵ�0������λ��
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

	//4.2������ɫ����
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, colorVbo));//ֻ�а���posVbo ��������������Ż����vbo���
	GL_CALL(glEnableVertexAttribArray(1)); // ��λ�����Էŵ�1������λ��
	GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

	GL_CALL(glBindVertexArray(0));//���
}

void prepareInterleavedBuffer() {
	//1 ׼����Interleaved����(λ��+��ɫ)
	float vertices[] = {
		   -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
			0.0f,  0.5f, 0.0f,  0.0f,  0.0f, 1.0f
	};
	//2 ����Ψһ��vbo
	GLuint vbo = 0;
	GL_CALL(glGenBuffers(1, &vbo));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,GL_STATIC_DRAW));

	//3 ��������vao
	GL_CALL(glGenVertexArrays(1, &vao));
	GL_CALL(glBindVertexArray(vao));

	//4 Ϊvao����λ�ú���ɫ��������Ϣ
	//4.1 λ��������Ϣ
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));

	//4.2 ��ɫ������Ϣ
	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),(void* )(3 * sizeof(float))));

	//5 ɨβ����
	GL_CALL(glBindVertexArray(0));
}

void prepareVAOForGLTriangles() {
	//1 ׼��positions
	float positions[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		 0.8f,  0.8f, 0.0f,
		 0.8f,  0.0f, 0.0f,
	};
	//2 ʹ����������posVbo
	GLuint posVbo;
	glGenBuffers(1, &posVbo);
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	//3 ����vao���Ұ�
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//4 ����λ������
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	GL_CALL(glBindVertexArray(0));

}

void prepareVAO2() {
	//1 ׼��positions colors
	//float positions[] = {
	//	-0.5f, -0.5f, 0.0f,
	//	0.5f, -0.5f, 0.0f,
	//	-0.5f,  0.5f, 0.0f,
	//	0.5f,  0.5f, 0.0f,
	//};

	//float colors[] = {
	//	1.0f, 0.0f,0.0f,
	//	0.0f, 1.0f,0.0f,
	//	0.0f, 0.0f,1.0f,
	//	0.5f, 0.5f,0.5f
	//};

	//float uvs[] = {
	//	0.0f, 0.0f,
	//	1.0f, 0.0f,
	//	0.0f, 1.0f,
	//	1.0f, 1.0f,
	//};

	//unsigned int indices[] = {
	//	0, 1, 2,
	//	2, 1, 3
	//};


	/*float positions[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f,
	};*/

	float positions[] = {
		-1.0f, 0.0f, 0.0f,
		 1.0f, 0.0f, 0.0f,
		 0.0f, 1.0f, 0.0f
	};

	float colors[] = {
		1.0f, 0.0f,0.0f,
		0.0f, 1.0f,0.0f,
		0.0f, 0.0f,1.0f,
	};

	float uvs[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.5f, 1.0f,
	};

	unsigned int indices[] = {
		0, 1, 2,
	};


	//2 VBO����
	GLuint posVbo, colorVbo, uvVbo;
	glGenBuffers(1, &posVbo);
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glGenBuffers(1, &colorVbo);
	glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	glGenBuffers(1, &uvVbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

	//3 EBO����
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//4 VAO����
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/*
	//��̬��ȡλ��
	//������shader����ʽдlocation = 0����ȡVAO������ID 
	//ֱ������������󶨵�ʱ�����glGetAttribLocation���ص�ID����
	GLuint posLocation = glGetAttribLocation(shader->mProgram, "aPos");
	GLuint colorLocation = glGetAttribLocation(shader->mProgram, "aColor");
	*/
	
	//5 ��vbo��ebo ��������������Ϣ
	//5.1 ����λ������������Ϣ
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);// ���������д��� �������ʡ�� ���ǲ�����
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	
	//5.2 ������ɫ������������
	glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	//5.3 ����uv������������
	glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

	//5.4 ����ebo����ǰ��vao
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // ���д�����Ѿ���ebo�󶨵��˵�ǰ��vao
	//���������д��� �������ﲻ��ʡ�� ��Ϊvbo��glVertexAttribPointer��������������е�ǰvbo�Ĳ�ѯ
	//eboû�����ִ��� ������glBindBuffer�Ļ�����������ȥ�鵱ǰ��û��ebo ������Ҫ��������
	glBindVertexArray(0);
}

void prepareVAO() {
	geometry = Geometry::createSphere(6.0f);
}

void prepareShader() {
	shader = new Shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
}

void prepareTexture() {
	texture = new Texture("assets/textures/goku.jpg", 0);
	texture2 = new Texture("assets/textures/hinata.jpg", 0);

	//grassTexture = new Texture("assets/textures/grass.jpg", 0);
	//landTexture = new Texture("assets/textures/land.jpg", 1);
	//noiseTexture = new Texture("assets/textures/noise.jpg", 2);
}

void prepareCamera() {
	float size = 6.0f;
	camera = new PerspectiveCamera(60.0f, (float)app->getWidth() / (float)app->getHeight(), 0.1f, 1000.0f);
	//camera = new OrthographicCamera(-size, size, size, -size, size, -size); 
	cameraControl = new TrackBallCameraControl();
	//cameraControl = new GameCameraControl();
	cameraControl->setCamera(camera);
	//cameraControl->setSensitivity(0.8f);
	//lookat:����һ��viewMatrix
	//eye:��ǰ��������ڵ�λ��
	//center:��ǰ�����������Ǹ���(���ȥ��ǰλ�ò��ǿ���ȥ������)
	//up:񷶥����
	//viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void prepareOrtho() {
	//ortho�����������������ϵ�µ�
	//�����������ϵ�µ�������,��һ��͸��ͶӰ�ĺ���
	//orthoMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 2.0f, -2.0f);
}

void preparePerspective() {
	//fovy:y�᷽������Ž�,����
	//aspect:��ƽ��ĺ��ݰٷֱ�
	//near:��ƽ�����
	//far:Զƽ�����
	perspectiveMatrix = glm::perspective(glm::radians(60.0f), (float)app->getWidth() / (float)app->getHeight(), 0.1f, 1000.0f);
}

void prepareState() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glClearDepth(0.0f); //������Ȼ��������ֵ Ĭ����1.0
}

void render() {
	/*
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//���������һ�д��� ����Ч����ֻ����һ֡������ �ӵڶ�֡��ʼ�ͱ���
	//ԭ���������Ѿ�����ǰVAO����Ϊ��0 ��ǰVAO���滹��֮ǰ�󶨵�EBO����Ϣ 
	//�������VAO��ʱ�� ��ʱEBO����Ϣ���� (�������Ϊ���Զ�����Ϊ��ǰ��EBO) ���Ե�һ֡���Ի�����
	//���ǵڶ�֡ʱ,ͨ�������������ǰEBO����Ϊ0,��ʱ����VAO,����Ҳ�ὫVAO�ϰ󶨵�EBO����Ϊ0.�����ò������ݱ���
	*/
	//ִ��opengl�����������
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	//1 �󶨵�ǰ��program
	shader->begin();
	//shader->setFloat("time", glfwGetTime());//Ҫ����uniform����һ��Ҫ��useprogram
	//shader->setFloat("speed", 2.0f);//Ҫ����uniform����һ��Ҫ��useprogram
	//shader->setVector3("uColor", 0.3f, 0.4f, 0.5f);
	//shader->setInt("grassSampler", 0);//sampler�ò����ĸ�����Ԫ
	//shader->setInt("landSampler", 1);
	//shader->setInt("noiseSampler", 2);
	//float color[] = {0.9f, 0.3f, 0.25f};
	//shader->setVector3("uColor", color);
	shader->setInt("sampler", 0);
	shader->setMatrix4x4("transform", transform);
	shader->setMatrix4x4("viewMatrix", camera->getViewMatrix());
	shader->setMatrix4x4("projectionMatrix", camera->getProjectionMatrix());
/*	shader->setMatrix4x4("viewMatrix", viewMatrix);
	shader->setMatrix4x4("projectionMatrix", perspectiveMatrix);*/ 

	texture->bind();
	////2 �󶨵�ǰ��vao
	//shader->setFloat("width", texture->getWidth());
	//shader->setFloat("height", texture->getHeight());
	glBindVertexArray(geometry->getVao());
	//3 ��������ָ��
	//glDrawArrays(GL_TRIANGLES, 0, 6); // ���Զ���ÿ�����㹹��һ�������εķ�ʽ����Ⱦ
	//glDrawArrays(GL_LINE_STRIP, 0, 6); 
	glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
	////�ڶ��λ���
	//texture2->bind();
	//transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, 0.0f, -1.0f));
	//shader->setMatrix4x4("transform", transform2);
	//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	shader->end();
	//�������ֵ��÷�ʽ
	//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(int) * 3));//��������λ��ƫ��
	//unsigned int indices[] = {
	//	0, 1, 2,
	//	2, 1, 3
	//};
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);//��Ҫע������İ�ebo��vao�Ĵ���
}

/* ��VBO��ϰ
void prepareVBO() {
	//1.����һ��vbo,��û�����������Դ�!!����ֻ�Ǵ�����һ���������͵Ķ���
	GLuint vbo = 0;
	GL_CALL(glGenBuffers(1, &vbo));
	//2.����һ��vbo
	GL_CALL(glDeleteBuffers(1, &vbo));

	//3.����n��vbo
	GLuint vboArr[] = { 0,0,0 };
	GL_CALL(glGenBuffers(3, vboArr));

	//4.����n��vbo
	GL_CALL(glDeleteBuffers(3, vboArr));
}

void prepare() {
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};
	//1 ����һ��vbo
	GLuint vbo = 0;
	GL_CALL(glGenBuffers(1, &vbo));
	//2 �󶨵�ǰvbo,��opengl״̬���ĵ�ǰvbo�����
	//GL_ARRAY_BUFFER:��ʾ��ǰvbo������
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	//3 ��ǰvbo��������, Ҳ���ڿ����Դ�
	//���ݴ�CPU���ڴ浽��GPU���Դ�
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
}

void prepareSingleBuffer() {
	//1.׼������λ�����ݺ���ɫ����
	float positions[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	float colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	//2.Ϊλ��&��ɫ���ݸ�������һ��vbo
	GLuint posVbo = 0, colorVbo = 0;
	GL_CALL(glGenBuffers(1, &posVbo));
	GL_CALL(glGenBuffers(1, &colorVbo));

	//3.�������ֿ���vbo�����������
	//position�������
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posVbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));
	//color�������
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, colorVbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));
}

void prepareInterleavedBuffer() {
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	GLuint vbo = 0;
	GL_CALL(glGenBuffers(1, &vbo));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
}



*/

int main() {
	if (!app->init(800, 600))
	{
		return -1;
	}

	app->setResizeCallback(OnResize);
	app->setKeyBoardCallback(OnKey);
	app->setMouseCallback(OnMouse);
	app->setCursorCallback(OnCursor);
	app->setScrollCallback(OnScroll);

	//����opengl�ӿ��Լ�������ɫ
	GL_CALL(glViewport(0, 0, 800, 600));
	GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));

	prepareShader();
	prepareVAO();
	prepareTexture();
	prepareCamera();
	preparePerspective();
	prepareState();
	//preTransform();
	while (app->update())
	{
		cameraControl->update();
		//doTransform();
		render();
	}

	app->destory();
	delete texture;

	return 0;
}