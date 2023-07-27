#ifndef FRAMEBUFFERFACTORY_H_
#define FRAMEBUFFERFACTORY_H_

#include <memory>

#include "glad/glad.h"

/**
 * @brief ֡����
 * 1.ÿ��֡���嶼�����Լ�һ�׻��壬ʹ��ǰ����ϣ�����ú��ʵ�λ������glClear�������Щ����
 */
class Framebuffer
{
public:
	Framebuffer(unsigned int id, unsigned int texId) : m_id(id), m_texId(texId) { }
	~Framebuffer();

	unsigned int id() { return m_id; }

	/**
	 * @brief ÿ����Ⱦǰ����󶨴���Ⱦ��֡������󣬾����VAOһ��
	 */
	void bindFramebuffer();
	void bindTexture();
	void bindTexture(unsigned int index);
	void bindTexture(unsigned int type, unsigned int index);

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
	struct FramebufferParam {
		int internalFormat3 = GL_RGB;
		int internalFormat4 = GL_RGBA;
	};
	FramebufferFactory() {}
	
	static std::shared_ptr<Framebuffer> createFramebuffer(const FramebufferParam& param = FramebufferParam());
	static std::shared_ptr<Framebuffer> createFramebuffer(int samples);
	static std::shared_ptr<Framebuffer> createDepthFb();
	static std::shared_ptr<Framebuffer> createCubeMapDepthFb();
};


#endif

