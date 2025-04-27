#pragma once
#include "material.h"
#include "../texture.h"

class PhongInstanceMaterial :public Material {
public:
	PhongInstanceMaterial();
	~PhongInstanceMaterial();

public:
	Texture*	mDiffuse{ nullptr };
	Texture*	mSpecularMask{ nullptr };
	float		mShiness{ 1.0f };
};