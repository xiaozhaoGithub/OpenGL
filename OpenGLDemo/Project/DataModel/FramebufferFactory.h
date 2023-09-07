#ifndef FRAMEBUFFERFACTORY_H_
#define FRAMEBUFFERFACTORY_H_

#include <memory>
#include <vector>

#include "glad/glad.h"

/**
 * @brief ֡����
 * 1.ÿ��֡���嶼�����Լ�һ�׻��壬ʹ��ǰ����ϣ�����ú��ʵ�λ������glClear�������Щ����
 */
class Framebuffer
{
public:
	Framebuffer(unsigned int id, unsigned int texId, unsigned int rbo);
	Framebuffer(unsigned int id, const std::vector<unsigned int>& texIds, unsigned int rbo);
	~Framebuffer();

	unsigned int id() { return m_id; }

	/**
	 * @brief ÿ����Ⱦǰ����󶨴���Ⱦ��֡������󣬾����VAOһ��
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

