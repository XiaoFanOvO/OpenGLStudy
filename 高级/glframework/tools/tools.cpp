#include "tools.h"

void Tools::decompose(
	glm::mat4 matrix, 
	glm::vec3& position, 
	glm::vec3& eulerAngle, 
	glm::vec3& scale) 
{
	//四元数，用来表示旋转变换
	glm::quat quaternion;
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(matrix, scale, quaternion, position, skew, perspective);

	//四元数-欧拉角
	glm::mat4 rotation = glm::toMat4(quaternion);
	glm::extractEulerAngleXYZ(rotation, eulerAngle.x, eulerAngle.y, eulerAngle.z);
	
	//解构出来的角度是弧度，转化为角度
	eulerAngle.x = glm::degrees(eulerAngle.x);
	eulerAngle.y = glm::degrees(eulerAngle.y);
	eulerAngle.z = glm::degrees(eulerAngle.z);
}


//传入一个相机的Projection * View矩阵乘积,得到对应视椎体的八个角点的世界坐标
std::vector<glm::vec4> Tools::getFrustumCornersWorldSpace(const glm::mat4& projView) {
	const auto inv = glm::inverse(projView);

	std::vector<glm::vec4> corners{};

	for (int x = 0; x < 2; x++)
	{
		for (int y = 0; y < 2; y++)
		{
			for (int z = 0; z < 2; z++)
			{
				glm::vec4 ndc = glm::vec4(2.0f * x - 1.0, 2.0f * y - 1.0, 2.0f * z - 1.0, 1.0); //将xyz从0-1变成-1-1
				glm::vec4 p_middle = inv * ndc;
				glm::vec4 p_world = p_middle / p_middle.w;
				corners.push_back(p_world);
			}
		}
	}

	return corners;
}