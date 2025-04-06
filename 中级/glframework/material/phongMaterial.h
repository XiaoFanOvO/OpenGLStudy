#pragma once
#include "material.h"
#include "../texture.h"


class PhongMaterial : public Material
{
public:
	PhongMaterial();
	~PhongMaterial();

	Texture* mDiffuse{ nullptr };
	Texture* mSpecularMask{ nullptr };
	float mShiness{ 1.0f };

private:

};

