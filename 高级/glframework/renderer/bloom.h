#pragma once
#include "../core.h"
#include "../framebuffer/framebuffer.h"
#include "../geometry.h"
#include "../shader.h"

class Bloom
{
public:
	Bloom(int width, int height, int minResolution = 32);
	~Bloom();

	void doBloom(Framebuffer* srcFbo);

	float mThreshold{ 1.0f };
	float mBloomRadius{ 0.1f };
	float mBloomAttenuation{ 1.0f };
	float mBloomIntensity{ 1.0f };

private:
	//下采样数组
	std::vector<Framebuffer*> mDownSamples{};
	//上采样数组
	std::vector<Framebuffer*> mUpSamples{};
	//初始化宽度高度,有多少级的下采样图片数量
	int mWidth{ 0 };
	int mHeight{ 0 };
	int mMipLevels{ 0 };//下采样图片数量

	Geometry* mQuad{ nullptr };

	Framebuffer* mOriginFbo{ nullptr };

	Shader* mExtractBrightShader{ nullptr };
	Shader* mUpSampleShader{ nullptr };
	Shader* mMergeShader{ nullptr };

	

	void extractBright(Framebuffer* src, Framebuffer* dst);
	void downSample(Framebuffer* src, Framebuffer* dst);
	void upSample(Framebuffer* target, Framebuffer* lowerResFbo, Framebuffer* higherResFbo);
	void merge(Framebuffer* target, Framebuffer* origin, Framebuffer* bloom);

};
