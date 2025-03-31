#pragma once

#include "../../glframework/core.h"

class Camera
{
public:
	Camera();
	~Camera();
	
	glm::mat4 getViewMatrix();
	virtual glm::mat4 getProjectionMatrix();
	virtual void scale(float deltaScale);

public:
	glm::vec3 mPosition{ 0.0f, 0.0f, 5.0f };
	glm::vec3 mUp{ 0.0f, 1.0f, 0.0f };   //本地坐标系的Y轴
	glm::vec3 mRight{ 1.0f,0.0f,0.0f };	 //本地做坐标系的x轴

};
