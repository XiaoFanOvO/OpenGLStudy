#pragma once
#include "light.h"
#include "../object.h"

class PointLight : public Light, public Object
{
public:
	PointLight();
	~PointLight();

	float mK1 = 1.0f;
	float mK2 = 1.0f;
	float mKc = 1.0f;

private:

};

