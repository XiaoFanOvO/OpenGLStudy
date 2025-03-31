#include "application.h"
#include<glad/glad.h>
#include <GLFW/glfw3.h> //������԰���  ����ͷ�ļ�����  cpp�ļ��ﲻ�����ͷ�ļ��ظ�����������
// ��Ϊcpp�ļ��ǵ�������� �������ֻ�������ӽ׶βŻᷢ������
//,hͷ�ļ����ڱ���׶ξͻᷢ������ ���Բ����ظ����������


//��ʼ��Application�ľ�̬����
Application* Application::mInstance = nullptr;
Application* Application::getInstance() {
	//���mInstance�Ѿ�ʵ������(new������)  ��ֱ�ӷ���
	//������Ҫ��new����,�ٷ���
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
	//1 ��ʼ��GLFW��������
	glfwInit();
	//1.1 ����OpenGL���汾�š��ΰ汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//1.2 ����OpenGL���ú���ģʽ����������Ⱦģʽ��
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//2 �����������
	mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGLStudy", NULL, NULL);
	if (mWindow == NULL)
	{
		false;
	}
	//**���õ�ǰ�������ΪOpenGL�Ļ�����̨
	glfwMakeContextCurrent(mWindow);

	//ʹ��glad�������е�ǰ�汾opengl�ĺ���
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallBack);
	//this ���ǵ�ǰȫ��Ψһ��Application����
	//����ǰ�����ȴ浽��ǰwindow����
	glfwSetWindowUserPointer(mWindow, this);

	//������Ӧ
	glfwSetKeyCallback(mWindow, keyCallBack);
	
	//������¼���Ӧ
	glfwSetMouseButtonCallback(mWindow, mouseCallBack);

	//����ƶ��¼���Ӧ
	glfwSetCursorPosCallback(mWindow, cursorCallBack);

	//��������Ϣ
	glfwSetScrollCallback(mWindow, scrollCallBack);

	return true;

}

bool Application::update() {
	if (glfwWindowShouldClose(mWindow))
	{
		return false;
	}
	//���ղ��ַ�������Ϣ
	//�����Ϣ�����Ƿ�����Ҫ�������ꡢ���̵���Ϣ
	//����еĻ��ͽ���Ϣ����������ն���
	glfwPollEvents();

	//�л�˫����
	glfwSwapBuffers(mWindow);

	return true;
}

void Application::destory() {
	//4 �˳�����ǰ���������
	glfwTerminate();
}

void Application::getCursorPosition(double* x, double* y) {
	glfwGetCursorPos(mWindow, x, y);
};

void Application::frameBufferSizeCallBack(GLFWwindow* window, int width, int height) {
	//std::cout << "resize" << std::endl;
	/*��ͨ����
	if (Application::getInstance()->mResizeCallback != nullptr)
	{
		Application::getInstance()->mResizeCallback(width, height);
	}*/
	Application* self =  (Application*)glfwGetWindowUserPointer(window);//��window�����ȡ�洢�Ķ���(��Ҫǿ������ת��)
	if (self->mResizeCallBack != nullptr)
	{
		self->mResizeCallBack(width, height);
	}
}

void Application::keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Application* self = (Application*)glfwGetWindowUserPointer(window);//��window�����ȡ�洢�Ķ���(��Ҫǿ������ת��)
	if (self->mKeyBoardCallBack != nullptr)
	{
		self->mKeyBoardCallBack(key, action, mods);
	}
}


void Application::mouseCallBack(GLFWwindow* window, int button, int action, int mods) {
	Application* self = (Application*)glfwGetWindowUserPointer(window);//��window�����ȡ�洢�Ķ���(��Ҫǿ������ת��)
	if (self->mMouseCallback != nullptr)
	{
		self->mMouseCallback(button, action, mods);
	}


};
//���λ�÷����仯ʱ
void Application::cursorCallBack(GLFWwindow* window, double xpos, double ypos) {
	Application* self = (Application*)glfwGetWindowUserPointer(window);//��window�����ȡ�洢�Ķ���(��Ҫǿ������ת��)
	if (self->mCursorCallback != nullptr)
	{
		self->mCursorCallback(xpos, ypos);
	}

}; 

//������Ϣ��xoffsetû��
void Application::scrollCallBack(GLFWwindow* window, double xoffset, double yoffset) {
	Application* self = (Application*)glfwGetWindowUserPointer(window);//��window�����ȡ�洢�Ķ���(��Ҫǿ������ת��)
	if (self->mScrollCallback != nullptr)
	{
		self->mScrollCallback(yoffset);
	}
};