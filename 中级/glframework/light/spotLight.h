#pragma once
#include "light.h"
#include "../object.h"

class SpotLight : public Light, public Object
{
public:
	SpotLight();
	~SpotLight();

	glm::vec3 mTargetDirection{ -1.0f }; // 看向的方向
	float mInnerAngle{ 0.0f }; //内部角度
	float mOuterAngle{ 0.0f }; //外部角度 

private:

};
