#pragma once

#include "../../glframework/core.h"
#include "camera.h"
#include <map>

class CameraControl
{
public:
	CameraControl();
	~CameraControl();

	//���ڼ̳�CameraControl������,�п��ܻ�ʵ���Լ����߼� ������������鷽��
	virtual void onMouse(int button, int action, double xpos, double ypos);
	virtual void onCursor(double xpos, double ypos);
	virtual void onKey(int key, int action, int mods);
	virtual void onScroll(float offset);//+1 -1

	//ÿһ֡��Ⱦ֮ǰ��Ҫ���е���,ÿһ֡���µ���Ϊ���Է�������
	virtual void update();
	void setCamera(Camera* camera) { mCamera = camera; }
	void setSensitivity(float s) { mSensitivity = s; }
	void setScaleSpeed(float s) { mScaleSpeed = s; }

protected:
	//1 ��갴��״̬
	bool mLeftMouseDown = false;
	bool mRightMouseDown = false;
	bool mMiddleMouseDown = false;

	//2 ��ǰ����λ��
	float mCurrentX = 0.0f;
	float mCurrentY = 0.0f;

	//3 ���ж�
	float mSensitivity = 0.2f;

	//4 ��¼������ذ����İ���״̬
	std::map<int, bool> mKeyMap;

	//5 �洢��ǰ���Ƶ���һ�������
	Camera* mCamera = nullptr;

	//6 ��¼������ŵ��ٶ�
	float mScaleSpeed = 0.2f;

};
