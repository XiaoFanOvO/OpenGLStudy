#pragma once
#include "core.h"

enum class ObjectType {
	Object,
	Mesh,
	Scene,
	InstancedMesh
};

class Object
{
public:
	Object();
	~Object();

	void setPosition(glm::vec3 pos);
	
	//������ת
	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);

	//������ת��
	void setAngleX(float angle);
	void setAngleY(float angle);
	void setAngleZ(float angle);

	void setScale(glm::vec3 scale);

	glm::vec3 getPosition() const { return mPosition; }

	glm::mat4 getModelMatrix() const;

	//���ӹ�ϵ
	void addChild(Object* obj);
	std::vector<Object*> getChildren();
	Object* getParent(); 

	//��ȡ������Ϣ
	ObjectType getType() const { return mType; }

protected:
	glm::vec3 mPosition{ 0.0f };

	//����xyz��������ת�ĽǶ�
	//unity��ת��׼:pitch yaw roll (x-y-z)
	float mAngleX{ 0.0f };
	float mAngleY{ 0.0f };
	float mAngleZ{ 0.0f };

	glm::vec3 mScale{ 1.0f };

	//���ӹ�ϵ
	std::vector<Object*> mChildren{};
	Object* mParent{ nullptr };

	//���ͼ�¼
	ObjectType mType;
};

