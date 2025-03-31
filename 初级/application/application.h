#pragma once
#include <iostream>
//#include <GLFW/glfw3.h> ����include ��Ϊmain�����Ѿ�����

#define app Application::getInstance()
class GLFWwindow; //����һ��class��Ŀ���Ƿ�ֹ���뱨�� ������Զ��滻��(cpp�������include)

using ResizeCallback = void(*)(int width, int height);
using KeyBoardCallback = void(*)(int key, int action, int mods);
using MouseCallback = void(*)(int button, int action, int mods);
using CursorCallback = void(*)(double xpos, double ypos);
using ScrollCallback = void(*)(double offset);

class Application
{
public:
	~Application();
	//���ڷ���ʵ���ľ�̬����
	static Application* getInstance();

	//��������ͷ���false ����true
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
	//C++���ں���ָ��
	static void frameBufferSizeCallBack(GLFWwindow* window, int width, int height);
	//������ʵ��һ��������Ϣ�ص�����
	//key:��ĸ������
	//scancode:��������
	//action:���»���̧��
	//mods:�Ƿ���shift��ctrl
	static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseCallBack(GLFWwindow* window, int button, int action, int mods);
	static void cursorCallBack(GLFWwindow* window, double xpos, double ypos); //���λ�÷����仯ʱ
	static void scrollCallBack(GLFWwindow* window, double xoffset, double yoffset); 

private:
	//ȫ��Ψһ�ľ�̬����ʵ��
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
