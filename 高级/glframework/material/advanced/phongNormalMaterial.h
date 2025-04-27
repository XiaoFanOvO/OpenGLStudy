#pragma once
#include "../material.h"
#include "../../texture.h"

class PhongNormalMaterial :public Material {
public:
	PhongNormalMaterial();
	~PhongNormalMaterial();

public:
	Texture*	mDiffuse{ nullptr };
	Texture*	mSpecularMask{ nullptr };
	Texture*	mNormalMap{ nullptr };
	float		mShiness{ 1.0f };
};