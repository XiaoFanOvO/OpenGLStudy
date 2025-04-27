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

	//这里是知道物体在自身空间的坐标
	//以及他在世界空间的平移旋转缩放  来求他在世界空间的坐标中间的变换矩阵
	//这里的平移旋转和缩放是针对的世界空间,不是他自身的空间!

	//unity：缩放 旋转 平移
	glm::mat4 transform{ 1.0f };

	transform = glm::scale(transform, mScale);

	//unity旋转标准：pitch yaw roll
	transform = glm::rotate(transform, glm::radians(mAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(mAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));

	transform = parentMatrix * glm::translate(glm::mat4(1.0f),mPosition) * transform;
	//这里不能写成glm::translate(transform,mPosition) 因为他是基于世界坐标系的平移 如果是transform的话就会是基于之前旋转的平移 导致平移的方向有问题
	 
	return transform;
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