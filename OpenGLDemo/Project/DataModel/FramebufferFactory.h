#ifndef FRAMEBUFFERFACTORY_H_
#define FRAMEBUFFERFACTORY_H_

#include <memory>
#include <vector>

#include "glad/glad.h"

/**
 * @brief 帧缓冲
 * 1.每个帧缓冲都有它自己一套缓冲，使用前我们希望设置合适的位，调用glClear，清除这些缓冲
 */
class Framebuffer
{
public:
	Framebuffer(unsigned int id, unsigned int texId, unsigned int rbo);
	Framebuffer(unsigned int id, const std::vector<unsigned int>& texIds, unsigned int rbo);
	~Framebuffer();

	unsigned int id() { return m_id; }

	/**
	 * @brief 每次渲染前，需绑定待渲染的帧缓冲对象，就像绑定VAO一样
	 */
	void bindFramebuffer();
	void bindTexture();
	void bindTexture(unsigned int activeTex);
	void bindTexture(unsigned int type, unsigned int activeTex);
	void bindTexture(unsigned int type, unsigned int activeTex, unsigned int index);
	void renderbufferStorage(unsigned int target, unsigned int internalformat, int width, int height);

	void blitFramebuffer(unsigned int targetFbo, unsigned int mask = GL_COLOR_BUFFER_BIT);

protected:
	unsigned int m_id;
	unsigned int m_rbo;
	std::vector<unsigned int> m_texIds;
};

class MuiltSampleFramebuffer : public Framebuffer
{
public:
	MuiltSampleFramebuffer(unsigned int id, unsigned int texId, unsigned int rbo) : Framebuffer(id, texId, rbo) {}
};

// Simple factory
class FramebufferFactory
{
public:
	struct FramebufferParam {
		int internalFormat = GL_RGB;
		int format = GL_RGB;
	};
	FramebufferFactory() {}
	
	static std::shared_ptr<Framebuffer> createFramebuffer(const FramebufferParam& param = FramebufferParam());
	static std::shared_ptr<Framebuffer> createFramebuffer(const FramebufferParam& param, int attachNum);
	static std::shared_ptr<Framebuffer> createFramebuffer(int samples);
	static std::shared_ptr<Framebuffer> createDepthFb();
	static std::shared_ptr<Framebuffer> createCubeMapDepthFb();
};


#endif

