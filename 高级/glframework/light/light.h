#pragma once
#include "../core.h"
#include "../object.h"
#include "shadow/shadow.h"

class Light:public Object {
public:
	Light();
	~Light();

public:
	glm::vec3	mColor{ 1.0f };
	float		mSpecularIntensity{ 1.0f };
	float		mIntensity{ 1.0 };
	Shadow* mShadow{ nullptr }; //这里存的是父类型,后面根据不同灯光转成不同的子类
};