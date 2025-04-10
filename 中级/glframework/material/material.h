#pragma once
#include "../core.h"

//使用C++的枚举类型
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

	//深度检测相关
	bool mDepthTest{ true };
	GLenum mDepthFunc{GL_LESS};
	bool mDepthWrite{ true };

	//polygonOffset相关
	bool mPolygonOffset{ false };
	unsigned int mPolygonOffsetType{ GL_POLYGON_OFFSET_FILL };
	float mFactor{ 0.0f };
	float mUnit{ 0.0f };
	
};

