#pragma once
#include "light.h"
#include "../object.h"

class PointLight :public Light , public Object{
public:
	PointLight();
	~PointLight();

public:
	float mK2 = 1.0f;
	float mK1 = 1.0f;
	float mKc = 1.0f;
};