#include "AdvancedLightingState.h"

#include <vector>

#include "Singleton.h"
#include "CommonDataDef.h"
#include "TextureHelper.h"

extern GLFWwindow* g_globalWindow;

namespace UCDD = UiCommonDataDef;

struct AdvancedLightingStateControlParam
{
	bool isBlinn = false;
	bool isGammaCorrection = false;
};

AdvancedLightingState::AdvancedLightingState()
	: m_controlParam(std::unique_ptr<AdvancedLightingStateControlParam>(new AdvancedLightingStateControlParam()))
{
	initTransformMatUniformBlock();

	m_woodFloorVAO = Singleton<TriangleVAOFactory>::instance()->createFloorVAO("skin/textures/wood.png");

	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("advanced_light_shader", "ShaderProgram/AdvancedLight/texture_shader.vs", "ShaderProgram/AdvancedLight/texture_shader.fs");
	m_shader->use();
	m_shader->setInt("sampler1", 0);

	glEnable(GL_DEPTH_TEST);
}

AdvancedLightingState::~AdvancedLightingState()
{
}

void AdvancedLightingState::draw()
{
	processInput();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	auto cameraWrapper = Singleton<CameraWrapper>::instance();
	cameraWrapper->setCursorVisible(false);

	// ģ�;�����Ҫ����ƽ�ơ����š���ת��һ������������ģ�;��󣬵õ�����ռ��еĶ���λ��

	// �۲������Ҫ������任���۲���������ռ�
	auto viewMat = cameraWrapper->lookAtMatrix();

	// ͶӰ����, �ü��ɼ����꣬������͸��Ч����Զ�����������η���Խ�󣬶���ԽС
	auto projectionMat = glm::perspective(glm::radians(cameraWrapper->fieldOfView()), float(UCDD::kViewportWidth) / UCDD::kViewportHeight, 0.1f, 100.0f);
	
	m_shader->use();

	glBindBuffer(GL_UNIFORM_BUFFER, m_transformMatUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMat));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMat));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	drawFloor();
}

void AdvancedLightingState::processInput()
{
	if (glfwGetKey(g_globalWindow, GLFW_KEY_B) == GLFW_PRESS) {
		m_controlParam->isBlinn = !m_controlParam->isBlinn;
	}

	if (glfwGetKey(g_globalWindow, GLFW_KEY_C) == GLFW_PRESS) {
		m_controlParam->isGammaCorrection = !m_controlParam->isGammaCorrection;

		// ����1, ʹ��gammaУ����ɥʧһЩ����Ȩ��
		//if (m_controlParam->isGammaCorrection) {
		//	glEnable(GL_FRAMEBUFFER_SRGB);
		//}
		//else {
		//	glDisable(GL_FRAMEBUFFER_SRGB);
		//}

		// ����2��ÿ��Ƭ����ɫ��������ɫ���ʱ��ȱ�㣺ÿ����ɫ����Ҫ��������ȡ�����ɫ�� 1/gamma ����
		m_shader->use();
		m_shader->setBool("isGammaCorrection", m_controlParam->isGammaCorrection);

		// ����3��������ڴ���ֻ��һ��Ƭ����ɫ������������ɫ���
		// TODO

		// sRGB����������ͼ��ʵ���Ծ���gammaУ���������ʵ��sRGB�ռ�����������������Կؼ�������У�����¹���̫����
		// �����ʽʹ��sRGB�����ʽ���Զ�����ɫУ�����Կռ��У�
	}
}

void AdvancedLightingState::drawFloor()
{
	auto cameraWrapper = Singleton<CameraWrapper>::instance();

	glStencilMask(0x00);
	m_shader->use();
	glm::mat4 modelMat = glm::mat4(1.0f);
	m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));
	m_shader->setVec("viewPos", cameraWrapper->cameraPos());
	m_shader->setVec("lightPos", glm::vec3(0.0f, 0.0f, 0.0f));
	m_shader->setBool("isBlinn", m_controlParam->isBlinn);

	m_woodFloorVAO->bindVAO();
	m_woodFloorVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}


