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

	// �����������������Ⱦ������󣩣�������һ���ڴ�λ�ã����ܹ���Ϊ֡�����һ������

	// 1.������
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

	// ��ά������Ϊ����Ļ��С�������ⲻ�Ǳ���ģ�
	// �յ������ṩ��֡������Ⱦʱ����������ɫ��������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, UCDD::kViewportWidth, UCDD::kViewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	// ���Ʒ�ʽĬ����GL_REPEAT��ȡ��������Ļ��һ�ߵ����أ�����һ�ߵ����ر���Ӧ�ö��������ز���Ӱ�죬����ܻ�����Ļ��Ե��������ֵ�����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// ���ӵ�֡������
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// ͬ���ɸ�����ȡ�ģ�建����������/��ϣ����˴�ʹ�ò�������������ʹ����Ⱦ������󸽼���ֻд���Ż���
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, UCDD::kViewportWidth, UCDD::kViewportHeight, 0, 
	//	GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL); // ���


	// 2.��Ⱦ������󸽼���ԭ����Ⱦ��ʽ�����������ʽת��������д��/�������ݵĴ洢���ʣ�ͨ������ֻд�����ɴӵ�ǰ�󶨵�֡�����ȡ���أ�glReadPixels��
	// ͨ����Ϊ��Ⱥ�ģ�帽�����󲿷�ʱ�䲻��Ҫ��ȡ�����е�ֵ��������Ҫ������
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, UCDD::kViewportWidth, UCDD::kViewportHeight);// ������Ⱥ�ģ����Ⱦ�������
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0); // �����ڴ�󣬿ɽ����Ⱦ����

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}
		
	// ��󣬼���Ĭ��֡������Ⱦ����֤���ǲ��᲻С����Ⱦ�������֡������
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<Framebuffer>(fbo, textureColorbuffer);
}
