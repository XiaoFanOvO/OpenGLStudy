#pragma once
#include "../core.h"

//使用C++的枚举类型
enum class MaterialType {
	PhongMaterial,
	WhiteMaterial,
	DepthMaterial,
	OpacityMaskMaterial,
	ScreenMaterial,
	CubeMaterial,
	PhongEnvMaterial,
	PhongInstanceMaterial
};

class Material
{
public:
	Material();
	~Material();
	
	MaterialType mType;

	//深度检测相关
	bool mDepthTest{ true };
	GLenum mDepthFunc{GL_LEQUAL};//小于等于的时候才能通过测试（天空盒深度为1也能通过了）
	bool mDepthWrite{ true };

	//polygonOffset相关
	bool mPolygonOffset{ false };
	unsigned int mPolygonOffsetType{ GL_POLYGON_OFFSET_FILL };
	float mFactor{ 0.0f };
	float mUnit{ 0.0f };

	//stencil相关
	bool mStencilTest{ false };
	unsigned int mSFail{ GL_KEEP };//模板测试失败了怎么办
	unsigned int mZFail{ GL_KEEP };//模板测试通过但是深度测试没通过怎么办
	unsigned int mZPass{ GL_KEEP };//模板+深度测试都通过了怎么办

	unsigned int mStencilMask{ 0xff };//用于控制模板写入

	unsigned int mStencilFunc{ GL_ALWAYS };
	unsigned int mStencilRef{ 0 };
	unsigned int mStencilFuncMask{ 0xff };

	//颜色混合
	bool mBlend{ false };
	unsigned int mSFactor{ GL_SRC_ALPHA };
	unsigned int mDFactor{ GL_ONE_MINUS_SRC_ALPHA };

	float mOpacity{ 1.0f };

	//face culling
	bool mFaceCulling{ false };
	unsigned int mFrontFace{ GL_CCW };
	unsigned int mCullFace{ GL_BACK };
	
};

