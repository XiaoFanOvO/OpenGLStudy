#include "bloom.h"

Bloom::Bloom(int width, int height, int minResolution) {
	mWidth = width;
	mHeight = height;

	float widthLevels = std::log2((float)width / (float)minResolution);
	float heightLevels = std::log2((float)height / (float)minResolution);

	mMipLevels = std::min(widthLevels, heightLevels);

	int w = mWidth, h = mHeight;
	for (int i = 0; i < mMipLevels; i++)
	{
		mDownSamples.push_back(Framebuffer::createHDRBloomFbo(w, h));
		w /= 2;
		h /= 2;
	}

	w = 4 * w, h = 4 * h;//重置数值
	//上采样比下采样少一次
	for (int i = 0; i < mMipLevels - 1; i++)
	{
		mUpSamples.push_back(Framebuffer::createHDRBloomFbo(w, h));
		w *= 2;
		h *= 2;
	}

	mQuad = Geometry::createScreenPlane();
	mOriginFbo = Framebuffer::createHDRBloomFbo(mWidth, mHeight);

	mExtractBrightShader = new Shader("assets/shaders/advanced/bloom/extractBright.vert", "assets/shaders/advanced/bloom/extractBright.frag");
	mUpSampleShader = new Shader("assets/shaders/advanced/bloom/upSample.vert", "assets/shaders/advanced/bloom/upSample.frag");
	mMergeShader = new Shader("assets/shaders/advanced/bloom/merge.vert", "assets/shaders/advanced/bloom/merge.frag");
}


Bloom::~Bloom() {

}

//这个FBO是离屏渲染的FBO,也就是要处理的FBO
void Bloom::doBloom(Framebuffer* srcFbo) {
	//1 保存原来的FBO以及ViewFort状态
	GLint preFbo;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &preFbo);
	GLint preViewport[4];
	glGetIntegerv(GL_VIEWPORT, preViewport);
	//2 将原始FBO进行备份保存
	glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFbo->mFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mOriginFbo->mFBO);
	glBlitFramebuffer(0, 0, srcFbo->mWidth, srcFbo->mHeight, 0, 0, mOriginFbo->mWidth, mOriginFbo->mHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	//3 提取亮部到downSample的第一个FBO上
	extractBright(srcFbo, mDownSamples[0]);
	//4 循环执行下采样
	for (int i = 1; i < mDownSamples.size(); i++)
	{
		auto src = mDownSamples[i - 1];
		auto dst = mDownSamples[i];
		downSample(src, dst);
	}
	//5 循环执行上采样
	int N = mDownSamples.size();
	auto lowerResFbo = mDownSamples[N - 1];//倒数第一张被上采样
	auto higherResFbo = mDownSamples[N - 2];//倒数第二张作为添加剂(这里要结合图来思考)
	upSample(mUpSamples[0], lowerResFbo, higherResFbo);//第一次上采样放在上采样数组的第0张上
	for (int i = 1; i < mUpSamples.size(); i++)
	{
		lowerResFbo = mUpSamples[i - 1];
		higherResFbo = mDownSamples[N - 2 - i];

		upSample(mUpSamples[i], lowerResFbo, higherResFbo);//最后一张上采样就包含了我们需要的bloom信息
	}
	//6 执行merge合并
	merge(srcFbo, mOriginFbo, mUpSamples[mUpSamples.size() - 1]);
	//7 恢复原始FBO以及Viewport状态
	glBindFramebuffer(GL_FRAMEBUFFER, preFbo);
	glViewport(preViewport[0], preViewport[1], preViewport[2], preViewport[3]);
}

//提取亮色信息
void Bloom::extractBright(Framebuffer* src, Framebuffer* dst) {
	glBindFramebuffer(GL_FRAMEBUFFER, dst->mFBO);
	glViewport(0, 0, dst->mWidth, dst->mHeight);
	glClear(GL_COLOR_BUFFER_BIT);//擦掉所有颜色

	mExtractBrightShader->begin();
	//设置uniform
	auto srcTex = src->mColorAttachment;
	srcTex->setUnit(0);
	srcTex->bind();
	mExtractBrightShader->setInt("srcTex", 0);
	mExtractBrightShader->setFloat("threshold", mThreshold);
	//绑定VAO
	glBindVertexArray(mQuad->getVao());
	//发送绘制命令
	glDrawElements(GL_TRIANGLES, mQuad->getIndicesCount(), GL_UNSIGNED_INT, 0);
	mExtractBrightShader->end();
}

//下采样
void Bloom::downSample(Framebuffer* src, Framebuffer* dst) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->mFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->mFBO);
	glBlitFramebuffer(0, 0, src->mWidth, src->mHeight, 0, 0, dst->mWidth, dst->mHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

//上采样
void Bloom::upSample(Framebuffer* target, Framebuffer* lowerResFbo, Framebuffer* higherResFbo) {
	glBindFramebuffer(GL_FRAMEBUFFER, target->mFBO);
	glViewport(0, 0, target->mWidth, target->mHeight);
	glClear(GL_COLOR_BUFFER_BIT);//擦掉所有颜色

	mUpSampleShader->begin();
	lowerResFbo->mColorAttachment->setUnit(0);
	lowerResFbo->mColorAttachment->bind();
	mUpSampleShader->setInt("lowerResTex", 0);

	higherResFbo->mColorAttachment->setUnit(1);
	higherResFbo->mColorAttachment->bind();
	mUpSampleShader->setInt("higherResTex", 1);

	mUpSampleShader->setFloat("bloomRadius", mBloomRadius);
	mUpSampleShader->setFloat("bloomAttenuation", mBloomAttenuation);

	//绑定VAO
	glBindVertexArray(mQuad->getVao());
	//发送绘制命令
	glDrawElements(GL_TRIANGLES, mQuad->getIndicesCount(), GL_UNSIGNED_INT, 0);

	mUpSampleShader->end();
}

void Bloom::merge(Framebuffer* target, Framebuffer* origin, Framebuffer* bloom) {
	glBindFramebuffer(GL_FRAMEBUFFER, target->mFBO);
	glViewport(0, 0, target->mWidth, target->mHeight);
	glClear(GL_COLOR_BUFFER_BIT);//擦掉所有颜色

	mMergeShader->begin();
	origin->mColorAttachment->setUnit(0);
	origin->mColorAttachment->bind();
	mMergeShader->setInt("originTex", 0);

	bloom->mColorAttachment->setUnit(1);
	bloom->mColorAttachment->bind();
	mMergeShader->setInt("bloomTex", 1);

	mMergeShader->setFloat("bloomIntensity", mBloomIntensity);

	//绑定VAO
	glBindVertexArray(mQuad->getVao());
	//发送绘制命令
	glDrawElements(GL_TRIANGLES, mQuad->getIndicesCount(), GL_UNSIGNED_INT, 0);

	mMergeShader->end();
}

