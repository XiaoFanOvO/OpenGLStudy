#pragma once
#include "../core.h"

//ʹ��C++��ö������
enum class MaterialType {
	PhongMaterial,
	WhiteMaterial

};

class Material
{
public:
	Material();
	~Material();
	
	MaterialType mType;

private:

};

