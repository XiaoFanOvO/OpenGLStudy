#include "application.h"
#include<glad/glad.h>
#include <GLFW/glfw3.h> //这里可以包含  但是头文件不行  cpp文件里不会出现头文件重复包含的问题
// 因为cpp文件是单独编译的 编译完后只有在链接阶段才会发生作用
//,h头文件是在编译阶段就会发生作用 所以才有重复定义的问题


//初始化Application的静态变量
Application* Application::mInstance = nullptr;
Application* Application::getInstance() {
	//如果mInstance已经实例化了(new出来了)  就直接返回
	//否则需要先new出来,再返回
	if (mInstance == nullptr)
	{
		mInstance = new Application();
	}
	return mInstance;
}


Application::Application() {

}

Application::~Application() {

}

bool Application::init(const int& width, const int& height) {
	mWidth = width;
	mHeight = height;
	//1 初始化GLFW基本环境
	glfwInit();
	//1.1 设置OpenGL主版本号、次版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//1.2 设置OpenGL启用核心模式（非立即渲染模式）
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//2 创建窗体对象
	mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGLStudy", NULL, NULL);
	if (mWindow == NULL)
	{
		false;
	}
	//**设置当前窗体对象为OpenGL的绘制舞台
	glfwMakeContextCurrent(mWindow);

	//使用glad加载所有当前版本opengl的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallBack);
	//this 就是当前全局唯一的Application对象
	//将当前对象先存到当前window里面
	glfwSetWindowUserPointer(mWindow, this);

	//键盘响应
	glfwSetKeyCallback(mWindow, keyCallBack);
	
	//鼠标点击事件响应
	glfwSetMouseButtonCallback(mWindow, mouseCallBack);

	//鼠标移动事件响应
	glfwSetCursorPosCallback(mWindow, cursorCallBack);

	//鼠标滚轮消息
	glfwSetScrollCallback(mWindow, scrollCallBack);

	return true;

}

bool Application::update() {
	if (glfwWindowShouldClose(mWindow))
	{
		return false;
	}
	//接收并分发窗体消息
	//检查消息队列是否有需要处理的鼠标、键盘等消息
	//如果有的话就将消息批量处理，清空队列
	glfwPollEvents();

	//切换双缓存
	glfwSwapBuffers(mWindow);

	return true;
}

void Application::destory() {
	//4 退出程序前做相关清理
	glfwTerminate();
}

void Application::getCursorPosition(double* x, double* y) {
	glfwGetCursorPos(mWindow, x, y);
};

void Application::frameBufferSizeCallBack(GLFWwindow* window, int width, int height) {
	//std::cout << "resize" << std::endl;
	/*普通方法
	if (Application::getInstance()->mResizeCallback != nullptr)
	{
		Application::getInstance()->mResizeCallback(width, height);
	}*/
	Application* self =  (Application*)glfwGetWindowUserPointer(window);//从window里面获取存储的对象(需要强制类型转换)
	if (self->mResizeCallBack != nullptr)
	{
		self->mResizeCallBack(width, height);
	}
}

void Application::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Application* self = (Application*)glfwGetWindowUserPointer(window);//从window里面获取存储的对象(需要强制类型转换)
	if (self->mKeyBoardCallBack != nullptr)
	{
		self->mKeyBoardCallBack(key, action, mods);
	}
}


void Application::mouseCallBack(GLFWwindow* window, int button, int action, int mods) {
	Application* self = (Application*)glfwGetWindowUserPointer(window);//从window里面获取存储的对象(需要强制类型转换)
	if (self->mMouseCallback != nullptr)
	{
		self->mMouseCallback(button, action, mods);
	}


};
//鼠标位置发生变化时
void Application::cursorCallBack(GLFWwindow* window, double xpos, double ypos) {
	Application* self = (Application*)glfwGetWindowUserPointer(window);//从window里面获取存储的对象(需要强制类型转换)
	if (self->mCursorCallback != nullptr)
	{
		self->mCursorCallback(xpos, ypos);
	}

}; 

//滚动消息的xoffset没用
void Application::scrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
	Application* self = (Application*)glfwGetWindowUserPointer(window);//从window里面获取存储的对象(需要强制类型转换)
	if (self->mScrollCallback != nullptr)
	{
		self->mScrollCallback(yoffset);
	}
};