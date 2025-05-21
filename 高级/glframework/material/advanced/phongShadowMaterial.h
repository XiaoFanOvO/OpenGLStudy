#pragma once
#include "../material.h"
#include "../../texture.h"

class PhongShadowMaterial :public Material {
public:
	PhongShadowMaterial();
	~PhongShadowMaterial();

public:
	Texture*	mDiffuse{ nullptr };
	Texture*	mSpecularMask{ nullptr };
	float		mShiness{ 1.0f };
	float		mBias{ 0.0f };
};