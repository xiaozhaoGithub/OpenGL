#include "FramebufferFactory.h"

#include "glad/glad.h"

namespace UCDD = UiCommonDataDef;

Framebuffer::Framebuffer(unsigned int id, unsigned int texId, unsigned int rbo)
	: Framebuffer(id, std::vector<unsigned int>(), rbo)
{
	m_texIds.emplace_back(texId);
}

Framebuffer::Framebuffer(unsigned int id, const std::vector<unsigned int>& texIds, unsigned int rbo)
	: m_id(id)
	, m_texIds(texIds)
	, m_rbo(rbo)
{
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_id);
}

void Framebuffer::bindFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void Framebuffer::bindTexture()
{
	glBindTexture(GL_TEXTURE_2D, m_texIds[0]);
}

void Framebuffer::bindTexture(unsigned int activeTex)
{
	bindTexture(GL_TEXTURE_2D, activeTex, 0);
}

void Framebuffer::bindTexture(unsigned int type, unsigned int activeTex)
{
	bindTexture(type, activeTex, 0);
}

void Framebuffer::bindTexture(unsigned int type, unsigned int activeTex, unsigned int index)
{
	glActiveTexture(activeTex);
	glBindTexture(type, m_texIds[index]);
}

void Framebuffer::renderbufferStorage(unsigned int target, unsigned int internalformat, int width, int height)
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(target, internalformat, width, height);
}

void Framebuffer::framebufferTexture2D(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level)
{
	glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

void Framebuffer::blitFramebuffer(unsigned int targetFbo, unsigned int mask)
{
	// 类型后缀：_FRAMEBUFFER
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFbo);

	// 位块传送
	glBlitFramebuffer(0, 0, UCDD::kViewportWidth, UCDD::kViewportHeight, 0, 0, UCDD::kViewportWidth, UCDD::kViewportHeight,
		mask,
		GL_NEAREST);
}

std::shared_ptr<Framebuffer> FramebufferFactory::createFramebuffer(const FramebufferParam& param)
{
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// 创建附件（纹理或渲染缓冲对象），附件是一个内存位置，它能够作为帧缓冲的一个缓冲

	// 1.纹理附件
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

	// 将维度设置为了屏幕大小（尽管这不是必须的）
	// 空的纹理，提供给帧缓冲渲染时，再填入颜色缓冲数据
	glTexImage2D(GL_TEXTURE_2D, 0, param.internalFormat, param.width, param.height, 0, param.format, GL_UNSIGNED_BYTE, NULL);
	// 环绕方式默认是GL_REPEAT，取到的是屏幕另一边的像素，而另一边的像素本不应该对中心像素产生影响，这可能会在屏幕边缘产生很奇怪的条纹
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 附加到帧缓冲上
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// 同样可附加深度、模板缓冲纹理（单独/组合），此处使用不适用纹理，而是使用渲染缓冲对象附件（只写，优化）
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, UCDD::kViewportWidth, UCDD::kViewportHeight, 0, 
	//	GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL); // 组合


	// 2.渲染缓冲对象附件（原生渲染格式，不做纹理格式转换，更快写入/复制数据的存储介质，通常都是只写，但可从当前绑定的帧缓冲获取像素：glReadPixels）
	// 通常作为深度和模板附件（大部分时间不需要读取缓冲中的值，即不需要采样）
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, param.width, param.height);// 创建深度和模板渲染缓冲对象
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0); // 分配内存后，可解绑渲染缓冲

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}
		
	// 解绑，激活默认帧缓冲渲染，保证我们不会不小心渲染到错误的帧缓冲上
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<Framebuffer>(fbo, textureColorbuffer, rbo);
}

std::shared_ptr<Framebuffer> FramebufferFactory::createFramebuffer(const FramebufferParam& param, int attachNum)
{
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// 创建附件（纹理或渲染缓冲对象），附件是一个内存位置，它能够作为帧缓冲的一个缓冲
	// 1.纹理附件
	unsigned int* colorbuffers = new unsigned int[attachNum];
	glGenTextures(attachNum, colorbuffers);

	std::vector<unsigned int > attachments;
	std::vector<unsigned int> texIds;

	for (int i = 0; i < attachNum; i++) {
		glBindTexture(GL_TEXTURE_2D, colorbuffers[i]);

		// 将维度设置为了屏幕大小（尽管这不是必须的）
		// 空的纹理，提供给帧缓冲渲染时，再填入颜色缓冲数据
		glTexImage2D(GL_TEXTURE_2D, 0, param.internalFormat, UCDD::kViewportWidth, UCDD::kViewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		
		// 环绕方式默认是GL_REPEAT，取到的是屏幕另一边的像素，而另一边的像素本不应该对中心像素产生影响，这可能会在屏幕边缘产生很奇怪的条纹
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// 附加到帧缓冲上
		unsigned int attachIndex = GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachIndex, GL_TEXTURE_2D, colorbuffers[i], 0);
		attachments.emplace_back(attachIndex);
		texIds.emplace_back(colorbuffers[i]);
	}
	delete[] colorbuffers;

	// 2.渲染缓冲对象附件（原生渲染格式，不做纹理格式转换，更快写入/复制数据的存储介质，通常都是只写，但可从当前绑定的帧缓冲获取像素：glReadPixels）
	// 通常作为深度和模板附件（大部分时间不需要读取缓冲中的值，即不需要采样）
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, UCDD::kViewportWidth, UCDD::kViewportHeight);// 创建深度和模板渲染缓冲对象
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0); // 分配内存后，可解绑渲染缓冲

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	// 注：第一个参数个数需要指定正确，否则导致部分缓冲附件没有被使用
	glDrawBuffers(attachNum, attachments.data());

	// 解绑，激活默认帧缓冲渲染，保证我们不会不小心渲染到错误的帧缓冲上
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<Framebuffer>(fbo, texIds, rbo);
}

std::shared_ptr<Framebuffer> FramebufferFactory::createFramebuffer(int samples)
{
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// 创建附件（纹理或渲染缓冲对象），附件是一个内存位置，它能够作为帧缓冲的一个缓冲

	// 1.纹理附件
	unsigned int colorbufferTexId;
	glGenTextures(1, &colorbufferTexId);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorbufferTexId);

	// 将维度设置为了屏幕大小（尽管这不是必须的）
	// 空的纹理，提供给帧缓冲渲染时，再填入颜色缓冲数据
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, UCDD::kViewportWidth, UCDD::kViewportHeight, GL_TRUE);
	// 环绕方式默认是GL_REPEAT，取到的是屏幕另一边的像素，而另一边的像素本不应该对中心像素产生影响，这可能会在屏幕边缘产生很奇怪的条纹
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 附加到帧缓冲上
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorbufferTexId, 0);
	// 同样可附加深度、模板缓冲纹理（单独/组合），此处使用不适用纹理，而是使用渲染缓冲对象附件（只写，优化）
	//glTexImage2D(GL_TEXTURE_2D_MULTISAMPLE, 0, GL_DEPTH24_STENCIL8, UCDD::kViewportWidth, UCDD::kViewportHeight, 0, 
	//	GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL); // 组合


	// 2.渲染缓冲对象附件（原生渲染格式，不做纹理格式转换，更快写入/复制数据的存储介质，通常都是只写，但可从当前绑定的帧缓冲获取像素：glReadPixels）
	// 通常作为深度和模板附件（大部分时间不需要读取缓冲中的值，即不需要采样）
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, UCDD::kViewportWidth, UCDD::kViewportHeight);// 创建深度和模板渲染缓冲对象
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0); // 分配内存后，可解绑渲染缓冲

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}

	// 解绑，激活默认帧缓冲渲染，保证我们不会不小心渲染到错误的帧缓冲上
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<MuiltSampleFramebuffer>(fbo, colorbufferTexId, rbo);
}

std::shared_ptr<Framebuffer> FramebufferFactory::createDepthFb()
{
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// 1.纹理附件
	unsigned int texturId;
	glGenTextures(1, &texturId);
	glBindTexture(GL_TEXTURE_2D, texturId);

	// 深度贴图分辨率1024 * 1024
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, UCDD::kShadowWidth, UCDD::kShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	// 环绕方式默认是GL_REPEAT，取到的是屏幕另一边的像素，而另一边的像素本不应该对中心像素产生影响，这可能会在屏幕边缘产生很奇怪的条纹
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 当采样深度贴图，光投影矩阵范围（-1，1）外，即大于1的坐标将都是阴影
	// 解决：通过限制边界值，当采样到深度贴图时，最近的深度值为1：当前深度均小于1，所以阴影值计算为0（rgba中r = 1.0）
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// 附加到帧缓冲上
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texturId, 0);

	// 无颜色缓冲的帧缓冲是不完整的，需要特别指明
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<Framebuffer>(fbo, texturId, 0);
}

std::shared_ptr<Framebuffer> FramebufferFactory::createCubeMapDepthFb()
{
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// 1.纹理附件
	unsigned int texturId;
	glGenTextures(1, &texturId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturId);

	// 深度贴图分辨率1024 * 1024
	for (unsigned int i = GL_TEXTURE_CUBE_MAP_POSITIVE_X; i <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; ++i) {
		glTexImage2D(i, 0, GL_DEPTH_COMPONENT, UCDD::kShadowWidth, UCDD::kShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	// 当采样深度贴图，光投影矩阵范围（-1，1）外，即大于1的坐标将都是阴影
	// 解决：通过限制边界值，当采样到深度贴图时，最近的深度值为1：当前深度均小于1，所以阴影值计算为0（rgba中r = 1.0）
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// 附加到帧缓冲上
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texturId, 0);

	// 无颜色缓冲的帧缓冲是不完整的，需要显示说明
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<Framebuffer>(fbo, texturId, 0);
}

