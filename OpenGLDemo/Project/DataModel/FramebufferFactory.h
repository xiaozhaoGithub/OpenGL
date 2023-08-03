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
	Framebuffer(unsigned int id, unsigned int texId) : m_id(id) { m_texIds.emplace_back(texId); }
	Framebuffer(unsigned int id, const std::vector<unsigned int>& texIds) : m_id(id) { m_texIds = texIds; }
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

	virtual void blitFramebuffer(unsigned int targetFbo) {};

protected:
	unsigned int m_id;
	std::vector<unsigned int> m_texIds;
};

class MuiltSampleFramebuffer : public Framebuffer
{
public:
	MuiltSampleFramebuffer(unsigned int id, unsigned int texId) : Framebuffer(id, texId) {}

	void blitFramebuffer(unsigned int targetFb) override;
};

// Simple factory
class FramebufferFactory
{
public:
	struct FramebufferParam {
		int internalFormat3 = GL_RGB;
		int internalFormat4 = GL_RGBA;
	};
	FramebufferFactory() {}
	
	static std::shared_ptr<Framebuffer> createFramebuffer(const FramebufferParam& param = FramebufferParam());
	static std::shared_ptr<Framebuffer> createFramebuffer(const FramebufferParam& param, int attachNum);
	static std::shared_ptr<Framebuffer> createFramebuffer(int samples);
	static std::shared_ptr<Framebuffer> createDepthFb();
	static std::shared_ptr<Framebuffer> createCubeMapDepthFb();
};


#endif

