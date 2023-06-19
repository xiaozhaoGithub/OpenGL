#ifndef FRAMEBUFFERFACTORY_H_
#define FRAMEBUFFERFACTORY_H_

#include <memory>

/**
 * @brief 帧缓冲
 * 1.每个帧缓冲都有它自己一套缓冲，使用前我们希望设置合适的位，调用glClear，清除这些缓冲
 */
class Framebuffer
{
public:
	Framebuffer(unsigned int id, unsigned int texId) : m_id(id), m_texId(texId) { }
	~Framebuffer();

	unsigned int id() { return m_id; }

	/**
	 * @brief 每次渲染前，需绑定待渲染的帧缓冲对象，就像绑定VAO一样
	 */
	void bindFramebuffer();
	void bindTexture();

	virtual void blitFramebuffer(unsigned int targetFbo) {};

protected:
	unsigned int m_id;
	unsigned int m_texId;
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
	FramebufferFactory() {}
	
	static std::shared_ptr<Framebuffer> createFramebuffer();
	static std::shared_ptr<Framebuffer> createFramebuffer(int samples);
};


#endif

