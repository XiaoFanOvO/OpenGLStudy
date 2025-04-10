#pragma once
#include "../core.h"

//ʹ��C++��ö������
enum class MaterialType {
	PhongMaterial,
	WhiteMaterial,
	DepthMaterial

};

class Material
{
public:
	Material();
	~Material();
	
	MaterialType mType;

	//��ȼ�����
	bool mDepthTest{ true };
	GLenum mDepthFunc{GL_LESS};
	bool mDepthWrite{ true };

	//polygonOffset���
	bool mPolygonOffset{ false };
	unsigned int mPolygonOffsetType{ GL_POLYGON_OFFSET_FILL };
	float mFactor{ 0.0f };
	float mUnit{ 0.0f };
	
};

