#include "object.h"

Object::Object() {
	mType = ObjectType::Object;
}

Object::~Object() {

}

void Object::setPosition(glm::vec3 pos) {
	mPosition = pos;
}

//������ת
void Object::rotateX(float angle) {
	mAngleX += angle;
}

void Object::rotateY(float angle) {
	mAngleY += angle;
}

void Object::rotateZ(float angle) {
	mAngleZ += angle;
}

void Object::setAngleX(float angle) {
	mAngleX = angle;
}

void Object::setAngleY(float angle) {
	mAngleY = angle;
}

void Object::setAngleZ(float angle) {
	mAngleZ = angle;
}

void Object::setScale(glm::vec3 scale) {
	mScale = scale;
}



glm::mat4 Object::getModelMatrix()const {
	//���Ȼ�ȡ���׵ı任����
	glm::mat4 parentMatrix{ 1.0f };
	if (mParent != nullptr) {
		parentMatrix = mParent->getModelMatrix();
	}

	//������֪������������ռ������
	//�Լ���������ռ��ƽ����ת����  ������������ռ�������м�ı任����
	//�����ƽ����ת����������Ե�����ռ�,����������Ŀռ�!

	//unity������ ��ת ƽ��
	glm::mat4 transform{ 1.0f };

	transform = glm::scale(transform, mScale);

	//unity��ת��׼��pitch yaw roll
	transform = glm::rotate(transform, glm::radians(mAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));

	transform = parentMatrix * glm::translate(glm::mat4(1.0f),mPosition) * transform;
	//���ﲻ��д��glm::translate(transform,mPosition) ��Ϊ���ǻ�����������ϵ��ƽ�� �����transform�Ļ��ͻ��ǻ���֮ǰ��ת��ƽ�� ����ƽ�Ƶķ���������
	 
	return transform;
}


void Object::addChild(Object* obj) {
	//1 ����Ƿ�����������������--���ص�����
	auto iter = std::find(mChildren.begin(), mChildren.end(), obj);
	if (iter != mChildren.end()) {
		std::cerr << "Duplicated Child added" << std::endl;
		return;
	}

	//2 ���뺢��
	mChildren.push_back(obj);

	//3 �����¼���ĺ������İְ���˭
	obj->mParent = this;
}

std::vector<Object*>  Object::getChildren() {
	return mChildren;
}

Object* Object::getParent() {
	return mParent;
}