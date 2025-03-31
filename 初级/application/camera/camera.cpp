#include "camera.h"

//����������ѳ�ʼ��������������
Camera::Camera() {

}

Camera::~Camera() {

}

glm::mat4 Camera::getViewMatrix() {
	//lookat
	//-eye:���λ��(ʹ��mPosition)
	//-center:��������������ĸ���(ʹ��front+mPosition)
	//-top:񷶥(ʹ��mUp����)
	glm::vec3 front = glm::cross(mUp, mRight);
	glm::vec3 center = mPosition + front;

	return glm::lookAt(mPosition, center, mUp);
}

glm::mat4 Camera::getProjectionMatrix() {
	return glm::identity<glm::mat4>();
};


void Camera::scale(float deltaScale) {
};