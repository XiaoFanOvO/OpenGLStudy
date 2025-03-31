#include "CameraControl.h"
#include <iostream>


/*
shader里面的ViewMatrix和ProjectionMatrix都来在camera
CameraControl里面修改的都是camera的mpos,mup,mright这三个参数,而且是实时修改
所以也会每帧影响到上面两个矩阵的值
*/

/*
这里的思想就是把三个轴和坐标点做单独的抽象,点是点,轴是轴
然后把移动也进行分解,画图得出是哪根轴变了或者位置点变了
要做旋转就是每根轴或者位置点单独乘上变换矩阵 将他们看成一个个个体 而不是整体
*/

CameraControl::CameraControl() {};
CameraControl::~CameraControl() {};

//由于继承CameraControl的子类,有可能会实现自己的逻辑
void CameraControl::onMouse(int button, int action, double xpos, double ypos) {
	//1 判断当前的按键是否按下
	bool pressed = action == GLFW_PRESS ? true : false;
	//2 如果按下,记录当前按下的位置
	if (pressed)
	{
		mCurrentX = xpos;
		mCurrentY = ypos;
	}

	//3 根据按下的鼠标按键不同,更改按键状态
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
	//过滤掉repeat的情况
	if (action == GLFW_REPEAT)
	{
		return;
	}
	//1 检测按下或者抬起,给到一个变量
	bool pressed = action == GLFW_PRESS ? true : false;

	//2 记录在keyMap中
	mKeyMap[key] = pressed;
};

//每一帧渲染之前都要进行调用,每一帧更新的行为可以放在这里
void CameraControl::update() {
	//std::cout << "update" << std::endl;
};

void CameraControl::onScroll(float offset) {
	
}