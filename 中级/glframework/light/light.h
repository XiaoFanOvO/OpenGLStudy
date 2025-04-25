#pragma once
#include "../core.h"


class Light
{
public:
	Light();
	~Light();

	glm::vec3 mColor{ 1.0f };
	float mSpecularIntensity{ 1.0f };
	float mIntensity{ 1.0f };//光照总体的强度
};

