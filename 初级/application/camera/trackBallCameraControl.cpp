#include "trackBallCameraControl.h"


TrackBallCameraControl::TrackBallCameraControl() {

};

TrackBallCameraControl::~TrackBallCameraControl() {

};

void TrackBallCameraControl::onCursor(double xpos, double ypos) {
	//����ƶ���ʱ�� �ᴫ�����µ�����xyλ��
	//mCurrentX��mCurrentY��������갴�µ�λ�û���˵��һ֡��λ��
	if (mLeftMouseDown)
	{
		//��������ĸ������
		//1 ���㾭�ߺ�γ����ת�������Ƕ�(�������п���)
		float deltaX = (xpos - mCurrentX) * mSensitivity;;
		float deltaY = (ypos - mCurrentY) * mSensitivity;

		//2 �ֿ�pitch��yaw���Լ���
		pitch(-deltaY);
		yaw(-deltaX);
	}

	if (mMiddleMouseDown)
	{
		float deltaX = (xpos - mCurrentX) * mMoveSpeed;;
		float deltaY = (ypos - mCurrentY) * mMoveSpeed;

		//���ﲻ�ı������� ֻ�ı������ƽ���λ��
		mCamera->mPosition += mCamera->mUp * deltaY;
		mCamera->mPosition -= mCamera->mRight * deltaX;
	}
	mCurrentX = xpos;
	mCurrentY = ypos;
};

void TrackBallCameraControl::pitch(float angle) {
	//����mRight����ת
	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), mCamera->mRight);
	//Ӱ�쵱ǰ�����up������λ��
	//���������仯
	mCamera->mUp = mat * glm::vec4(mCamera->mUp, 0.0f);//�����������0 ��Ϊ��һ������ vec4����vec3 ֻ��xyz	
	mCamera->mPosition = mat * glm::vec4(mCamera->mPosition, 1.0f);//�����������1 ��Ϊ��һ��λ��
};

void TrackBallCameraControl::yaw(float angle) {
	//������������ϵ��Y����ת
	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	mCamera->mUp = mat * glm::vec4(mCamera->mUp, 0.0f);
	mCamera->mRight = mat * glm::vec4(mCamera->mRight, 1.0f);
	mCamera->mPosition = mat * glm::vec4(mCamera->mPosition, 1.0f);
};

void TrackBallCameraControl::onScroll(float offset) {
	mCamera->scale(offset * mScaleSpeed);
};