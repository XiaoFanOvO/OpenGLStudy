#pragma once 
#include "core.h"

enum class ObjectType {
	Object,
	Mesh,
	InstancedMesh,
	Scene
};

class Object {
public:
	Object();
	~Object();
	
	void setPosition(glm::vec3 pos);

	//增量旋转
	void rotateX(float angle); 
	void rotateY(float angle); 
	void rotateZ(float angle); 

	//设置旋转角度
	void setAngleX(float angle);
	void setAngleY(float angle);
	void setAngleZ(float angle);

	void setScale(glm::vec3 scale);

	glm::vec3 getPosition()const { return mPosition; }

	glm::mat4 getModelMatrix()const;

	glm::vec3 getDirection() const;

	//父子关系
	void addChild(Object* obj);
	std::vector<Object*>  getChildren();
	Object* getParent();

	//获取类型信息
	ObjectType getType()const { return mType; }

protected:
	glm::vec3 mPosition{ 0.0f };
	
	//unity旋转标准：pitch yaw roll
	float mAngleX{ 0.0f };
	float mAngleY{ 0.0f };
	float mAngleZ{ 0.0f };

	glm::vec3 mScale{ 1.0f };

	//父子关系
	std::vector<Object*>	mChildren{};
	Object*					mParent{ nullptr };

	//类型记录
	ObjectType	mType;
};