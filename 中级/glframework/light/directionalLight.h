#pragma once
#include "light.h"

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	~DirectionalLight();

	glm::vec3 mDirection{ -1.0f };

private:

};
