#pragma once

#include "camera.h"

class OrthographicCamera :public Camera {
public:
	OrthographicCamera(float l, float r, float b, float t, float n, float f);
	~OrthographicCamera();

	glm::mat4 getProjectionMatrix()override;

	void scale(float deltaScale)override;

public:
	float mL = 0.0f;
	float mR = 0.0f;
	float mT = 0.0f;
	float mB = 0.0f;

	float mScale{ 0.0f };
};