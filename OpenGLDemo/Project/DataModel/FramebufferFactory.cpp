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
	// ���ͺ�׺��_FRAMEBUFFER
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFbo);

	// λ�鴫��
	glBlitFramebuffer(0, 0, UCDD::kViewportWidth, UCDD::kViewportHeight, 0, 0, UCDD::kViewportWidth, UCDD::kViewportHeight,
		mask,
		GL_NEAREST);
}

std::shared_ptr<Framebuffer> FramebufferFactory::createFramebuffer(const FramebufferParam& param)
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
	glTexImage2D(GL_TEXTURE_2D, 0, param.internalFormat, param.width, param.height, 0, param.format, GL_UNSIGNED_BYTE, NULL);
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

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, param.width, param.height);// ������Ⱥ�ģ����Ⱦ�������
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0); // �����ڴ�󣬿ɽ����Ⱦ����

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer is not complete!" << std::endl;
	}
		
	// ��󣬼���Ĭ��֡������Ⱦ����֤���ǲ��᲻С����Ⱦ�������֡������
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<Framebuffer>(fbo, textureColorbuffer, rbo);
}

std::shared_ptr<Framebuffer> FramebufferFactory::createFramebuffer(const FramebufferParam& param, int attachNum)
{
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// �����������������Ⱦ������󣩣�������һ���ڴ�λ�ã����ܹ���Ϊ֡�����һ������
	// 1.������
	unsigned int* colorbuffers = new unsigned int[attachNum];
	glGenTextures(attachNum, colorbuffers);

	std::vector<unsigned int > attachments;
	std::vector<unsigned int> texIds;

	for (int i = 0; i < attachNum; i++) {
		glBindTexture(GL_TEXTURE_2D, colorbuffers[i]);

		// ��ά������Ϊ����Ļ��С�������ⲻ�Ǳ���ģ�
		// �յ������ṩ��֡������Ⱦʱ����������ɫ��������
		glTexImage2D(GL_TEXTURE_2D, 0, param.internalFormat, UCDD::kViewportWidth, UCDD::kViewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		
		// ���Ʒ�ʽĬ����GL_REPEAT��ȡ��������Ļ��һ�ߵ����أ�����һ�ߵ����ر���Ӧ�ö��������ز���Ӱ�죬����ܻ�����Ļ��Ե��������ֵ�����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// ���ӵ�֡������
		unsigned int attachIndex = GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachIndex, GL_TEXTURE_2D, colorbuffers[i], 0);
		attachments.emplace_back(attachIndex);
		texIds.emplace_back(colorbuffers[i]);
	}
	delete[] colorbuffers;

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
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	// ע����һ������������Ҫָ����ȷ�������²��ֻ��帽��û�б�ʹ��
	glDrawBuffers(attachNum, attachments.data());

	// ��󣬼���Ĭ��֡������Ⱦ����֤���ǲ��᲻С����Ⱦ�������֡������
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_shared<Framebuffer>(fbo, texIds, rbo);
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

	return std::make_shared<MuiltSampleFramebuffer>(fbo, colorbufferTexId, rbo);
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

	return std::make_shared<Framebuffer>(fbo, texturId, 0);
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

	return std::make_shared<Framebuffer>(fbo, texturId, 0);
}

