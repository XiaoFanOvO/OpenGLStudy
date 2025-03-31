#include "CameraControl.h"
#include <iostream>


/*
shader�����ViewMatrix��ProjectionMatrix������camera
CameraControl�����޸ĵĶ���camera��mpos,mup,mright����������,������ʵʱ�޸�
����Ҳ��ÿ֡Ӱ�쵽�������������ֵ
*/

/*
�����˼����ǰ��������������������ĳ���,���ǵ�,������
Ȼ����ƶ�Ҳ���зֽ�,��ͼ�ó����ĸ�����˻���λ�õ����
Ҫ����ת����ÿ�������λ�õ㵥�����ϱ任���� �����ǿ���һ�������� ����������
*/

CameraControl::CameraControl() {};
CameraControl::~CameraControl() {};

//���ڼ̳�CameraControl������,�п��ܻ�ʵ���Լ����߼�
void CameraControl::onMouse(int button, int action, double xpos, double ypos) {
	//1 �жϵ�ǰ�İ����Ƿ���
	bool pressed = action == GLFW_PRESS ? true : false;
	//2 �������,��¼��ǰ���µ�λ��
	if (pressed)
	{
		mCurrentX = xpos;
		mCurrentY = ypos;
	}

	//3 ���ݰ��µ���갴����ͬ,���İ���״̬
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		mLeftMouseDown = pressed;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		mRightMouseDown = pressed;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mMiddleMouseDown = pressed;
		break;
	}
};
void CameraControl::onCursor(double xpos, double ypos) {



};

void CameraControl::onKey(int key, int action, int mods) {
	//���˵�repeat�����
	if (action == GLFW_REPEAT)
	{
		return;
	}
	//1 ��ⰴ�»���̧��,����һ������
	bool pressed = action == GLFW_PRESS ? true : false;

	//2 ��¼��keyMap��
	mKeyMap[key] = pressed;
};

//ÿһ֡��Ⱦ֮ǰ��Ҫ���е���,ÿһ֡���µ���Ϊ���Է�������
void CameraControl::update() {
	//std::cout << "update" << std::endl;
};

void CameraControl::onScroll(float offset) {
	
}