#include "FramebufferFactory.h"

#include "glad/glad.h"
#include "CommonDataDef.h"

namespace UCDD = UiCommonDataDef;

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
	glBindTexture(GL_TEXTURE_2D, m_texId);
}

std::shared_ptr<Framebuffer> FramebufferFactory::createFramebuffer()
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, UCDD::kViewportWidth, UCDD::kViewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, UCDD::kViewportWidth, UCDD::kViewportHeight);// 创建深度和模板渲染缓冲对象
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0); // 分配内存后，可解绑渲染缓冲

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}
		
	// 解绑，激活默认帧缓冲渲染，保证我们不会不小心渲染到错误的帧缓冲上
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<Framebuffer>(fbo, textureColorbuffer);
}
