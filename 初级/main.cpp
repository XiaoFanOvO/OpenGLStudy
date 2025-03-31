#include <iostream>

#include "glframework/core.h"
#include "glframework/shader.h"
#include <string>
#include <assert.h>//断言
#include "wrapper/checkError.h"
#include "application/application.h"
#include "glframework/texture.h"

//引入相机+控制器
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
//glm::mat4 orthoMatrix(1.0f);//正交投影矩阵
//glm::mat4 perspectiveMatrix(1.0f);//透视投影矩阵

PerspectiveCamera* camera = nullptr;
TrackBallCameraControl* cameraControl = nullptr;
//OrthographicCamera* orthCamera = nullptr;
glm::mat4 viewMatrix(1.0f);
glm::mat4 perspectiveMatrix(1.0f);

Geometry* geometry = nullptr;

//声明且实现一个响应窗体大小变化的函数
void frameBufferSizeCallBack(GLFWwindow* window, int width, int height) {
	//std::cout << "窗体最新大小：" << width << " , " << height << std::endl;
	//让视口大小和窗口大小一起改变
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

//鼠标按下或抬起
void OnMouse(int button, int action, int mods) {
	//std::cout << "OnMouse" << std::endl;
	double x, y;
	app->getCursorPosition(&x, &y);
	cameraControl->onMouse(button, action, x, y);
}

//鼠标移动
void OnCursor(double xpos, double ypos) {
	//std::cout << "OnCursor" << std::endl;
	cameraControl->onCursor(xpos, ypos);
}

//鼠标滚轮
void OnScroll(double offset) {
	cameraControl->onScroll(offset);
}

//旋转变换
//默认以原点为中心
void doRotationTransform() {
	//构建一个旋转矩阵,绕着z轴旋转45度角
	//rotate函数:用于生成旋转矩阵
	//bug1: rotate这里必须得到一个float类型的角度
	//bug2: rotate函数接受的不是角度(degree)  而是弧度(radians)
	//radians函数也是模版函数,切记传入float类型数据
	transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
}

//平移变换
void doTranslationTransform() {
	transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f));
}

//缩放变换
void doScaleTransform() {
	transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 1.0f));
}

void preTransform() {
	//目标二:先平移再叠加旋转
	//平移只做一次,不能放到whiletrue里面
	//transform = glm::translate(transform, glm::vec3(0.6f, 0.0f, 0.0f));

	//目标三:先做一个旋转,只旋转一次
	//float angle = 45.0f;
	//transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

	//目标四:先做一次缩放
	transform = glm::scale(transform, glm::vec3(0.5f, 1.0f, 1.0f));
}

void doTransform() {
	//目标一:旋转的三角形
	//float angle = 1.0f;
	//transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));//旋转会叠加 因为是基于上一次变换

	//目标二:先平移再叠加旋转
	//旋转每帧都做
	//float angle = 1.0f;
	//transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

	//目标三:先旋转再叠加平移
	//1 preTransform里面先做一个旋转
	//2 每一帧平移
	//transform = glm::translate(transform, glm::vec3(0.01f, 0.0f, 0.0f));

	//目标四:先做一次缩放再叠加平移
	transform = glm::translate(transform, glm::vec3(0.01f, 0.0f, 0.0f));
}

//旋转动画
float angle = 0.0f;
void doRotation() {
	angle += 2.0f;
	//每一帧都会重新构建一个旋转矩阵(与上一个旋转矩阵没有关系)
	transform = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
}

void prepareSingleBuffer() {
	//1.准备positions colors数据
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

	//2.使用数据生成两个vbo  posVBO  colorVBO
	GLuint posVbo, colorVbo;
	GL_CALL(glGenBuffers(1, &posVbo));
	GL_CALL(glGenBuffers(1, &colorVbo));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posVbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, colorVbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));

	//3.生成vao并且绑定
	GLuint vao = 0;
	GL_CALL(glGenVertexArrays(1, &vao));
	GL_CALL(glBindVertexArray(vao));

	//4.分别将位置/颜色属性的描述信息加入vao当中
	//4.1 描述位置属性
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posVbo));//只有绑定了posVbo 下面的属性描述才会与此vbo相关
	GL_CALL(glEnableVertexAttribArray(0)); // 将位置属性放到0号描述位上
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

	//4.2描述颜色属性
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, colorVbo));//只有绑定了posVbo 下面的属性描述才会与此vbo相关
	GL_CALL(glEnableVertexAttribArray(1)); // 将位置属性放到1号描述位上
	GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

	GL_CALL(glBindVertexArray(0));//解绑
}

void prepareInterleavedBuffer() {
	//1 准备好Interleaved数据(位置+颜色)
	float vertices[] = {
		   -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
			0.0f,  0.5f, 0.0f,  0.0f,  0.0f, 1.0f
	};
	//2 创建唯一的vbo
	GLuint vbo = 0;
	GL_CALL(glGenBuffers(1, &vbo));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,GL_STATIC_DRAW));

	//3 创建并绑定vao
	GL_CALL(glGenVertexArrays(1, &vao));
	GL_CALL(glBindVertexArray(vao));

	//4 为vao加入位置和颜色的描述信息
	//4.1 位置描述信息
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));

	//4.2 颜色描述信息
	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),(void* )(3 * sizeof(float))));

	//5 扫尾工作
	GL_CALL(glBindVertexArray(0));
}

void prepareVAOForGLTriangles() {
	//1 准备positions
	float positions[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		 0.8f,  0.8f, 0.0f,
		 0.8f,  0.0f, 0.0f,
	};
	//2 使用数据生成posVbo
	GLuint posVbo;
	glGenBuffers(1, &posVbo);
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	//3 生成vao并且绑定
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//4 描述位置属性
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	GL_CALL(glBindVertexArray(0));

}

void prepareVAO2() {
	//1 准备positions colors
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


	//2 VBO创建
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

	//3 EBO创建
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//4 VAO创建
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/*
	//动态获取位置
	//不用在shader里显式写location = 0来获取VAO的属性ID 
	//直接这里在下面绑定的时候采用glGetAttribLocation返回的ID即可
	GLuint posLocation = glGetAttribLocation(shader->mProgram, "aPos");
	GLuint colorLocation = glGetAttribLocation(shader->mProgram, "aColor");
	*/
	
	//5 绑定vbo和ebo 加入属性描述信息
	//5.1 加入位置属性描述信息
	glBindBuffer(GL_ARRAY_BUFFER, posVbo);// 上面有这行代码 这里可以省略 但是不建议
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	
	//5.2 加入颜色属性描述数据
	glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	//5.3 加入uv属性描述数据
	glBindBuffer(GL_ARRAY_BUFFER, uvVbo);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);

	//5.4 加入ebo到当前的vao
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // 这行代码就已经将ebo绑定到了当前的vao
	//上面有这行代码 但是这里不能省略 因为vbo有glVertexAttribPointer这个方法主动进行当前vbo的查询
	//ebo没有这种代码 不调用glBindBuffer的话他不会主动去查当前绑没绑ebo 所以需要主动调用
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
	//lookat:生成一个viewMatrix
	//eye:当前摄像机所在的位置
	//center:当前摄像机看向的那个点(点减去当前位置才是看过去的向量)
	//up:穹顶向量
	//viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void prepareOrtho() {
	//ortho的数据是摄像机坐标系下的
	//给摄像机坐标系下的六个点,画一个透视投影的盒子
	//orthoMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 2.0f, -2.0f);
}

void preparePerspective() {
	//fovy:y轴方向的视张角,弧度
	//aspect:近平面的横纵百分比
	//near:近平面距离
	//far:远平面距离
	perspectiveMatrix = glm::perspective(glm::radians(60.0f), (float)app->getWidth() / (float)app->getHeight(), 0.1f, 1000.0f);
}

void prepareState() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glClearDepth(0.0f); //设置深度缓存的清理值 默认是1.0
}

void render() {
	/*
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//如果加上这一行代码 最后的效果是只会有一帧画出来 从第二帧开始就报错
	//原因是上面已经将当前VAO设置为了0 当前VAO里面还有之前绑定的EBO的信息 
	//在下面绑定VAO的时候 这时EBO的信息还在 (可以理解为会自动设置为当前的EBO) 所以第一帧可以画出来
	//但是第二帧时,通过这个方法将当前EBO设置为0,此时绑着VAO,所以也会将VAO上绑定的EBO设置为0.导致拿不到数据报错
	*/
	//执行opengl画布清理操作
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	//1 绑定当前的program
	shader->begin();
	//shader->setFloat("time", glfwGetTime());//要设置uniform变量一定要先useprogram
	//shader->setFloat("speed", 2.0f);//要设置uniform变量一定要先useprogram
	//shader->setVector3("uColor", 0.3f, 0.4f, 0.5f);
	//shader->setInt("grassSampler", 0);//sampler该采样哪个纹理单元
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
	////2 绑定当前的vao
	//shader->setFloat("width", texture->getWidth());
	//shader->setFloat("height", texture->getHeight());
	glBindVertexArray(geometry->getVao());
	//3 发出绘制指令
	//glDrawArrays(GL_TRIANGLES, 0, 6); // 会自动以每三个点构成一个三角形的方式做渲染
	//glDrawArrays(GL_LINE_STRIP, 0, 6); 
	glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, 0);
	////第二次绘制
	//texture2->bind();
	//transform2 = glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, 0.0f, -1.0f));
	//shader->setMatrix4x4("transform", transform2);
	//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	shader->end();
	//另外两种调用方式
	//glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(int) * 3));//带三个单位的偏移
	//unsigned int indices[] = {
	//	0, 1, 2,
	//	2, 1, 3
	//};
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);//需要注释上面的绑定ebo到vao的代码
}

/* 绑定VBO练习
void prepareVBO() {
	//1.创建一个vbo,还没有真正分配显存!!这里只是创建了一个描述类型的对象
	GLuint vbo = 0;
	GL_CALL(glGenBuffers(1, &vbo));
	//2.销毁一个vbo
	GL_CALL(glDeleteBuffers(1, &vbo));

	//3.创建n个vbo
	GLuint vboArr[] = { 0,0,0 };
	GL_CALL(glGenBuffers(3, vboArr));

	//4.销毁n个vbo
	GL_CALL(glDeleteBuffers(3, vboArr));
}

void prepare() {
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};
	//1 生成一个vbo
	GLuint vbo = 0;
	GL_CALL(glGenBuffers(1, &vbo));
	//2 绑定当前vbo,到opengl状态机的当前vbo插槽上
	//GL_ARRAY_BUFFER:表示当前vbo这个插槽
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	//3 向当前vbo传输数据, 也是在开辟显存
	//数据从CPU的内存到了GPU的显存
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
}

void prepareSingleBuffer() {
	//1.准备定点位置数据和颜色数据
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
	//2.为位置&颜色数据各自生成一个vbo
	GLuint posVbo = 0, colorVbo = 0;
	GL_CALL(glGenBuffers(1, &posVbo));
	GL_CALL(glGenBuffers(1, &colorVbo));

	//3.给两个分开的vbo各自填充数据
	//position填充数据
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posVbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));
	//color填充数据
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

	//设置opengl视口以及清理颜色
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