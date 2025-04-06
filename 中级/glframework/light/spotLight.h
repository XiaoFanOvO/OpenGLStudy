#pragma once
#include "light.h"
#include "../object.h"

class SpotLight : public Light, public Object
{
public:
	SpotLight();
	~SpotLight();

	glm::vec3 mTargetDirection{ -1.0f }; // ����ķ���
	float mInnerAngle{ 0.0f }; //�ڲ��Ƕ�
	float mOuterAngle{ 0.0f }; //�ⲿ�Ƕ� 

private:

};
