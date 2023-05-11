#ifndef FRAMEBUFFERFACTORY_H_
#define FRAMEBUFFERFACTORY_H_

#include <memory>

/**
 * @brief ֡����
 * 1.ÿ��֡���嶼�����Լ�һ�׻��壬ʹ��ǰ����ϣ�����ú��ʵ�λ������glClear�������Щ����
 */
class Framebuffer
{
public:
	Framebuffer(unsigned int id, unsigned int texId) : m_id(id), m_texId(texId) { }
	~Framebuffer();

	/**
	 * @brief ÿ����Ⱦǰ����󶨴���Ⱦ��֡������󣬾����VAOһ��
	 */
	void bindFramebuffer();
	void bindTexture();

protected:
	unsigned int m_id;
	unsigned int m_texId;
};

// Simple factory
class FramebufferFactory
{
public:
	FramebufferFactory() {}
	
	static std::shared_ptr<Framebuffer> createFramebuffer();
};


#endif

