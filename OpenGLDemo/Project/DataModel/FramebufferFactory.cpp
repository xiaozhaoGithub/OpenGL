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

void Framebuffer::bindTexture(unsigned int index)
{
	glActiveTexture(index);
	glBindTexture(GL_TEXTURE_2D, m_texId);
}

void Framebuffer::bindTexture(unsigned int type, unsigned int index)
{
	glActiveTexture(index);
	glBindTexture(type, m_texId);
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

std::shared_ptr<Framebuffer> FramebufferFactory::createFramebuffer(int samples)
{
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// �����������������Ⱦ������󣩣�������һ���ڴ�λ�ã����ܹ���Ϊ֡�����һ������

	// 1.������
	unsigned int colorbufferTexId;
	glGenTextures(1, &colorbufferTexId);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorbufferTexId);

	// ��ά������Ϊ����Ļ��С�������ⲻ�Ǳ���ģ�
	// �յ������ṩ��֡������Ⱦʱ����������ɫ��������
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, UCDD::kViewportWidth, UCDD::kViewportHeight, GL_TRUE);
	// ���Ʒ�ʽĬ����GL_REPEAT��ȡ��������Ļ��һ�ߵ����أ�����һ�ߵ����ر���Ӧ�ö��������ز���Ӱ�죬����ܻ�����Ļ��Ե��������ֵ�����
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// ���ӵ�֡������
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorbufferTexId, 0);
	// ͬ���ɸ�����ȡ�ģ�建����������/��ϣ����˴�ʹ�ò�������������ʹ����Ⱦ������󸽼���ֻд���Ż���
	//glTexImage2D(GL_TEXTURE_2D_MULTISAMPLE, 0, GL_DEPTH24_STENCIL8, UCDD::kViewportWidth, UCDD::kViewportHeight, 0, 
	//	GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL); // ���


	// 2.��Ⱦ������󸽼���ԭ����Ⱦ��ʽ�����������ʽת��������д��/�������ݵĴ洢���ʣ�ͨ������ֻд�����ɴӵ�ǰ�󶨵�֡�����ȡ���أ�glReadPixels��
	// ͨ����Ϊ��Ⱥ�ģ�帽�����󲿷�ʱ�䲻��Ҫ��ȡ�����е�ֵ��������Ҫ������
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, UCDD::kViewportWidth, UCDD::kViewportHeight);// ������Ⱥ�ģ����Ⱦ�������
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0); // �����ڴ�󣬿ɽ����Ⱦ����

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}

	// ��󣬼���Ĭ��֡������Ⱦ����֤���ǲ��᲻С����Ⱦ�������֡������
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<MuiltSampleFramebuffer>(fbo, colorbufferTexId);
}

std::shared_ptr<Framebuffer> FramebufferFactory::createDepthFb()
{
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// 1.������
	unsigned int texturId;
	glGenTextures(1, &texturId);
	glBindTexture(GL_TEXTURE_2D, texturId);

	// �����ͼ�ֱ���1024 * 1024
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, UCDD::kShadowWidth, UCDD::kShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	// ���Ʒ�ʽĬ����GL_REPEAT��ȡ��������Ļ��һ�ߵ����أ�����һ�ߵ����ر���Ӧ�ö��������ز���Ӱ�죬����ܻ�����Ļ��Ե��������ֵ�����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// �����������ͼ����ͶӰ����Χ��-1��1���⣬������1�����꽫������Ӱ
	// �����ͨ�����Ʊ߽�ֵ���������������ͼʱ����������ֵΪ1����ǰ��Ⱦ�С��1��������Ӱֵ����Ϊ0��rgba��r = 1.0��
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// ���ӵ�֡������
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texturId, 0);

	// ����ɫ�����֡�����ǲ������ģ���Ҫ�ر�ָ��
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<Framebuffer>(fbo, texturId);
}

std::shared_ptr<Framebuffer> FramebufferFactory::createCubeMapDepthFb()
{
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// 1.������
	unsigned int texturId;
	glGenTextures(1, &texturId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturId);

	// �����ͼ�ֱ���1024 * 1024
	for (unsigned int i = GL_TEXTURE_CUBE_MAP_POSITIVE_X; i <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; ++i) {
		glTexImage2D(i, 0, GL_DEPTH_COMPONENT, UCDD::kShadowWidth, UCDD::kShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	// �����������ͼ����ͶӰ����Χ��-1��1���⣬������1�����꽫������Ӱ
	// �����ͨ�����Ʊ߽�ֵ���������������ͼʱ����������ֵΪ1����ǰ��Ⱦ�С��1��������Ӱֵ����Ϊ0��rgba��r = 1.0��
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// ���ӵ�֡������
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texturId, 0);

	// ����ɫ�����֡�����ǲ������ģ���Ҫ��ʾ˵��
	glReadBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<Framebuffer>(fbo, texturId);
}

void MuiltSampleFramebuffer::blitFramebuffer(unsigned int targetFbo)
{
	// ���ͺ�׺��_FRAMEBUFFER
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFbo);

	// λ�鴫��
	glBlitFramebuffer(0, 0, UCDD::kViewportWidth, UCDD::kViewportHeight, 0, 0, UCDD::kViewportWidth, UCDD::kViewportHeight,
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST);
}

