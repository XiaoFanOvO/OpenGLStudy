#pragma once
#include "material.h"
#include "../texture.h"

class GrassInstanceMaterial :public Material {
public:
	GrassInstanceMaterial();
	~GrassInstanceMaterial();

public:
	Texture*	mDiffuse{ nullptr };
	Texture*	mOpacityMask{ nullptr };
	float		mShiness{ 1.0f };

	//草地贴图特性
	float		mUVScale{ 1.0f };
	float		mBrightness{ 1.0f };

	//风力相关
	float		mWindScale{ 0.1f };
	glm::vec3	mWindDirection{ 1.0, 1.0,1.0 };
	float		mPhaseScale{ 1.0f };

	//云朵相关
	Texture*	mCloudMask{ nullptr };
	glm::vec3	mCloudWhiteColor{ 0.576, 1.0, 0.393 };
	glm::vec3	mCloudBlackColor{ 0.994, 0.3, 0.426 };
	float		mCloudUVScale{ 1.0f };
	float		mCloudSpeed{ 0.1f };
	float		mCloudLerp{ 0.5f };
};