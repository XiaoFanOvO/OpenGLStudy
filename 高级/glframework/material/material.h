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
	PhongInstanceMaterial,
	GrassInstanceMaterial,

	PhongNormalMaterial,
	PhongParallaxMaterial
};

class Material {
public:
	Material();
	~Material();

public:
	MaterialType mType;

	//深度检测相关
	bool	mDepthTest{ true };
	GLenum	mDepthFunc{ GL_LEQUAL };
	bool	mDepthWrite{ true };

	//polygonOffset相关
	bool			mPolygonOffset{ false };
	unsigned int	mPolygonOffsetType{ GL_POLYGON_OFFSET_FILL };
	float			mFactor{ 0.0f };
	float			mUnit{ 0.0f };

	//stencil相关
	//glEnable(GL_STENCIL_TEST);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	//glStencilMask(0xFF);//保证了模板缓冲可以被清理
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);//如何测试
	bool			mStencilTest{ false };

	unsigned int	mSFail{ GL_KEEP };	//模板测试失败了怎么办
	unsigned int	mZFail{ GL_KEEP };	//模板测试通过但是深度检测没通过怎么办
	unsigned int	mZPass{ GL_KEEP };	//模板+深度测试都通过了怎么办

	unsigned int	mStencilMask{ 0xFF };//用于控制模板写入

	unsigned int    mStencilFunc{ GL_ALWAYS };
	unsigned int	mStencilRef{ 0 };
	unsigned int	mStencilFuncMask{ 0xFF };

	//颜色混合
	bool			mBlend{ false };
	unsigned int	mSFactor{ GL_SRC_ALPHA };
	unsigned int	mDFactor{ GL_ONE_MINUS_SRC_ALPHA };

	float			mOpacity{ 1.0f };

	//Face Culling
	bool			mFaceCulling{ false };
	unsigned int	mFrontFace{ GL_CCW };
	unsigned int	mCullFace{ GL_BACK };
};