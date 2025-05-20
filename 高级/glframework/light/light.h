#pragma once
#include "../core.h"
#include "../object.h"

class Light : public Object {
public:
	Light();
	~Light();

public:
	glm::vec3	mColor{ 1.0f };
	float		mSpecularIntensity{ 1.0f };
	float		mIntensity{ 1.0 };
};