#include "trackBallCameraControl.h"


TrackBallCameraControl::TrackBallCameraControl() {

};

TrackBallCameraControl::~TrackBallCameraControl() {

};

void TrackBallCameraControl::onCursor(double xpos, double ypos) {
	//鼠标移动的时候 会传入最新的鼠标的xy位置
	//mCurrentX和mCurrentY里面存的鼠标按下的位置或者说上一帧的位置
	if (mLeftMouseDown)
	{
		//调整相机的各类参数
		//1 计算经线和纬线旋转的增量角度(正负都有可能)
		float deltaX = (xpos - mCurrentX) * mSensitivity;;
		float deltaY = (ypos - mCurrentY) * mSensitivity;

		//2 分开pitch跟yaw各自计算
		pitch(-deltaY);
		yaw(-deltaX);
	}

	if (mMiddleMouseDown)
	{
		float deltaX = (xpos - mCurrentX) * mMoveSpeed;;
		float deltaY = (ypos - mCurrentY) * mMoveSpeed;

		//这里不改变三根轴 只改变摄像机平面的位置
		mCamera->mPosition += mCamera->mUp * deltaY;
		mCamera->mPosition -= mCamera->mRight * deltaX;
	}
	mCurrentX = xpos;
	mCurrentY = ypos;
};

void TrackBallCameraControl::pitch(float angle) {
	//绕着mRight在旋转
	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), mCamera->mRight);
	//影响当前相机的up向量和位置
	//都是增量变化
	mCamera->mUp = mat * glm::vec4(mCamera->mUp, 0.0f);//齐次这里填上0 因为是一个向量 vec4给到vec3 只给xyz	
	mCamera->mPosition = mat * glm::vec4(mCamera->mPosition, 1.0f);//齐次这里填上1 因为是一个位置
};

void TrackBallCameraControl::yaw(float angle) {
	//绕着世界坐标系的Y轴旋转
	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	mCamera->mUp = mat * glm::vec4(mCamera->mUp, 0.0f);
	mCamera->mRight = mat * glm::vec4(mCamera->mRight, 1.0f);
	mCamera->mPosition = mat * glm::vec4(mCamera->mPosition, 1.0f);
};

void TrackBallCameraControl::onScroll(float offset) {
	mCamera->scale(offset * mScaleSpeed);
};