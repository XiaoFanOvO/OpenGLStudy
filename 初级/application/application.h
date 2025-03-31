#pragma once
#include <iostream>
//#include <GLFW/glfw3.h> 不能include 因为main里面已经包含

#define app Application::getInstance()
class GLFWwindow; //声明一个class的目的是防止编译报错 后面会自动替换掉(cpp里面会有include)

using ResizeCallback = void(*)(int width, int height);
using KeyBoardCallback = void(*)(int key, int action, int mods);
using MouseCallback = void(*)(int button, int action, int mods);
using CursorCallback = void(*)(double xpos, double ypos);
using ScrollCallback = void(*)(double offset);

class Application
{
public:
	~Application();
	//用于访问实例的静态函数
	static Application* getInstance();

	//出现问题就返回false 否则true
	bool init(const int& width = 800, const int& height = 600); 
	bool update();
	void destory();
	
	uint32_t getWidth() const { return mWidth; }
	uint32_t getHeight() const { return mHeight; }
	void getCursorPosition(double* x, double* y);

	void setResizeCallback(ResizeCallback callback) { mResizeCallBack = callback; }
	void setKeyBoardCallback(KeyBoardCallback callback) { mKeyBoardCallBack = callback; }
	void setMouseCallback(MouseCallback callback) { mMouseCallback = callback; }
	void setCursorCallback(CursorCallback callback) { mCursorCallback = callback; }
	void setScrollCallback(ScrollCallback callback) { mScrollCallback = callback; }

private:
	//C++类内函数指针
	static void frameBufferSizeCallBack(GLFWwindow* window, int width, int height);
	//声明且实现一个键盘消息回调函数
	//key:字母按键码
	//scancode:物理按键码
	//action:按下还是抬起
	//mods:是否有shift或ctrl
	static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseCallBack(GLFWwindow* window, int button, int action, int mods);
	static void cursorCallBack(GLFWwindow* window, double xpos, double ypos); //鼠标位置发生变化时
	static void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset); 

private:
	//全局唯一的静态变量实例
	static Application* mInstance;

	uint32_t mWidth{ 0 };
	uint32_t mHeight{ 0 };
	GLFWwindow* mWindow{ nullptr };

	ResizeCallback mResizeCallBack{nullptr};
	KeyBoardCallback mKeyBoardCallBack{ nullptr };
	MouseCallback mMouseCallback{ nullptr };
	CursorCallback mCursorCallback{ nullptr };
	ScrollCallback mScrollCallback{ nullptr };


	Application();  
};
