#pragma once
#include "material.h"
#include "../texture.h"


class PhongEnvMaterial : public Material
{
public:
	PhongEnvMaterial();
	~PhongEnvMaterial();

	Texture* mDiffuse{ nullptr };
	Texture* mSpecularMask{ nullptr };
	float mShiness{ 1.0f };
	Texture* mEnv{ nullptr };

private:

};

