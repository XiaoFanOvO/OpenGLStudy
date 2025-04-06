#include "tools.h"

Tools::Tools()
{
}

Tools::~Tools()
{
}



 void Tools::decompose(glm::mat4 matrix, glm::vec3& position, glm::vec3& eulerAngle, glm::vec3& scale) {
	//四元数,用来表示旋转变换
	glm::quat quaternion;
	glm::vec3 skew;
	glm::vec4 perspective;
	
	glm::decompose(matrix, scale, quaternion, position, skew, perspective);

	//四元数-欧拉角
	glm::mat4 rotation = glm::toMat4(quaternion);
	glm::extractEulerAngleXYZ(rotation, eulerAngle.x, eulerAngle.y, eulerAngle.z);

	//解构出来的角度是弧度,需要转化为角度信息
	eulerAngle.x = glm::degrees(eulerAngle.x);
	eulerAngle.y = glm::degrees(eulerAngle.y);
	eulerAngle.z = glm::degrees(eulerAngle.z);
	
}