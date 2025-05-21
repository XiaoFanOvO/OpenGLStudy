#include "object.h"

Object::Object() {
	mType = ObjectType::Object;
}

Object::~Object() {

}

void Object::setPosition(glm::vec3 pos) {
	mPosition = pos;
}

//增量旋转
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
	//首先获取父亲的变换矩阵
	glm::mat4 parentMatrix{ 1.0f };
	if (mParent != nullptr) {
		parentMatrix = mParent->getModelMatrix();
	}


	//unity：缩放 旋转 平移
	glm::mat4 transform{ 1.0f };

	transform = glm::scale(transform, mScale);

	//unity旋转标准：pitch yaw roll
	transform = glm::rotate(transform, glm::radians(mAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));

	transform = parentMatrix * glm::translate(glm::mat4(1.0f),mPosition) * transform;

	return transform;
}

glm::vec3 Object::getDirection()const {
	auto modelMatrix = glm::mat3(getModelMatrix());
	auto dir = glm::normalize(-modelMatrix[2]);

	return dir;
}


void Object::addChild(Object* obj) {
	//1 检查是否曾经加入过这个孩子--返回迭代器
	auto iter = std::find(mChildren.begin(), mChildren.end(), obj);
	if (iter != mChildren.end()) {
		std::cerr << "Duplicated Child added" << std::endl;
		return;
	}

	//2 加入孩子
	mChildren.push_back(obj);

	//3 告诉新加入的孩子他的爸爸是谁
	obj->mParent = this;
}

std::vector<Object*>  Object::getChildren() {
	return mChildren;
}

Object* Object::getParent() {
	return mParent;
}