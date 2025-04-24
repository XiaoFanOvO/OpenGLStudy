#pragma once
#include "material.h"
#include "../texture.h"


class GrassInstanceMaterial : public Material
{
public:
	GrassInstanceMaterial();
	~GrassInstanceMaterial();

	Texture* mDiffuse{ nullptr };
	Texture* mOpacityMask{ nullptr };
	float mShiness{ 1.0f };

	//²İµØÌùÍ¼ÌØĞÔ
	float mUVScale{ 1.0f };
	float mBrightness{ 1.0f };

private:

};

