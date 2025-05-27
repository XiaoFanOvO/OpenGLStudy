#pragma once

#include "camera.h"

class PerspectiveCamera :public Camera {
public:
	PerspectiveCamera(float fovy, float aspect, float near, float far);
	~PerspectiveCamera();

	glm::mat4 getProjectionMatrix()override;

	void scale(float deltaScale)override;

public:
	float mFovy = 0.0f;//这是角度,不是弧度
	float mAspect = 0.0f;
};