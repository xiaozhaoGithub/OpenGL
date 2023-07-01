#include "AdvancedLightingState.h"

#include <vector>

#include "Singleton.h"
#include "CommonDataDef.h"
#include "TextureHelper.h"

extern GLFWwindow* g_globalWindow;

namespace UCDD = UiCommonDataDef;

struct AdvancedLightingStateControlParam
{
	int stateKey = GLFW_KEY_1;
	bool isBlinn = false;
	bool isGammaCorrection = false;
};

AdvancedLightingState::AdvancedLightingState()
	: m_controlParam(std::unique_ptr<AdvancedLightingStateControlParam>(new AdvancedLightingStateControlParam()))
{
	m_cameraWrapper = Singleton<CameraWrapper>::instance();

	initTransformMatUniformBlock();

	m_woodFloorVAO = Singleton<TriangleVAOFactory>::instance()->createFloorVAO("skin/textures/wood.png");
	m_cubeVAO = Singleton<TriangleVAOFactory>::instance()->createTexCubeVAO();
	m_quadVAO = Singleton<TriangleVAOFactory>::instance()->createQuadVAO();

	auto shaderFactory = Singleton<ShaderFactory>::instance();
	m_shader = shaderFactory->shaderProgram("advanced_light_shader", "ShaderProgram/AdvancedLight/texture_shader.vs", "ShaderProgram/AdvancedLight/texture_shader.fs");
	m_shader->use();
	m_shader->setInt("sampler1", 0);

	m_depthMapShader = shaderFactory->shaderProgram("depth_map", "ShaderProgram/AdvancedLight/depth_map.vs", "ShaderProgram/AdvancedLight/depth_map.fs");
	m_depthMapShader->use();
	m_depthMapShader->setInt("sampler1", 0);

	m_debugDepthShader = shaderFactory->shaderProgram("debug_depth_shader", "ShaderProgram/Advanced/screen_texture_shader.vs", "ShaderProgram/AdvancedLight/debug_depth_shader.fs");
	m_debugDepthShader->use();
	m_debugDepthShader->setInt("depthMap", 0);	
	
	m_shadowShader = shaderFactory->shaderProgram("texture_shadow", "ShaderProgram/AdvancedLight/texture_shadow.vs", "ShaderProgram/AdvancedLight/texture_shadow.fs");
	m_shadowShader->use();
	m_shadowShader->setInt("diffuseTexture", 0);
	m_shadowShader->setInt("shadowMap", 1);

	m_depthMapFb = FramebufferFactory::createDepthFb();

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

	m_cameraWrapper->setCursorVisible(false);

	// ģ�;�����Ҫ����ƽ�ơ����š���ת��һ������������ģ�;��󣬵õ�����ռ��еĶ���λ��

	// �۲������Ҫ������任���۲���������ռ�
	auto viewMat = m_cameraWrapper->lookAtMatrix();

	// ͶӰ����, �ü��ɼ����꣬������͸��Ч����Զ�����������η���Խ�󣬶���ԽС
	auto projectionMat = glm::perspective(glm::radians(m_cameraWrapper->fieldOfView()), float(UCDD::kViewportWidth) / UCDD::kViewportHeight, 0.1f, 100.0f);
	
	m_shader->use();

	glBindBuffer(GL_UNIFORM_BUFFER, m_transformMatUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMat));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMat));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	switch (m_controlParam->stateKey) {
	case GLFW_KEY_1: {
		drawFloor(m_shader);
		break;
	}
	case GLFW_KEY_2: {
		drawDepthMap();
		break;
	}
	case GLFW_KEY_3: {
		drawShadow();
		break;
	}
	default:
		break;
	}
}

void AdvancedLightingState::processInput()
{
	for (int key = GLFW_KEY_1; key <= GLFW_KEY_9; key++) {
		if (glfwGetKey(g_globalWindow, key) == GLFW_PRESS) {
			m_controlParam->stateKey = key;
		}
	}

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

void AdvancedLightingState::drawFloor(std::shared_ptr<AbstractShader> shader)
{
	glStencilMask(0x00);
	shader->use();
	glm::mat4 modelMat = glm::mat4(1.0f);
	shader->setMatrix("modelMat", modelMat);
	shader->setVec("viewPos", m_cameraWrapper->cameraPos());
	shader->setVec("lightPos", glm::vec3(0.0f, 0.0f, 0.0f));
	shader->setBool("isBlinn", m_controlParam->isBlinn);

	m_woodFloorVAO->bindVAO();
	m_woodFloorVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void AdvancedLightingState::drawCube()
{
	m_cubeVAO->bindVAO();
	m_cubeVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void AdvancedLightingState::drawDepthMap()
{
	// light space mat
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

	float nearPlane = 1.0f;
	float farPlane = 7.5f;

	auto lightViewMat = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto lightProjMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

	m_depthMapShader->use();
	m_depthMapShader->setMatrix("lightSpaceMat", lightProjMat * lightViewMat);

	// 1.��Ⱦ�����ͼ���Ǵӹ��͸��ͼ����Ⱦ�������������������Ӱ
	// ��Ӱ��ͼ����������ԭ����Ⱦ�ĳ�����ͨ���Ǵ��ڷֱ��ʣ����Ų�ͬ�ķֱ��ʣ�������Ҫ�ı��ӿڣ�viewport���Ĳ�������Ӧ��Ӱ��ͼ�ĳߴ硣
	// ����������˸����ӿڲ��������������ͼҪô̫СҪô�Ͳ�������
	glViewport(0, 0, UCDD::kShadowWidth, UCDD::kShadowHeight);
	m_depthMapFb->bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT); // ���֡����������Ȼ���

	drawFloor(m_shader);

	m_depthMapShader->use();

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(0.0f, 1.5f, 0.0));
	modelMat = glm::scale(modelMat, glm::vec3(0.5f));
	m_depthMapShader->setMatrix("modelMat", modelMat);
	drawCube();

	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(2.0f, 0.0f, 1.0));
	modelMat = glm::scale(modelMat, glm::vec3(0.5f));
	m_depthMapShader->setMatrix("modelMat", modelMat);
	drawCube();

	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(-1.0f, 0.0f, 2.0));
	modelMat = glm::rotate(modelMat, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	modelMat = glm::scale(modelMat, glm::vec3(0.25f));
	m_depthMapShader->setMatrix("modelMat", modelMat);
	drawCube();
	
	// reset viewport
	glViewport(0, 0, UCDD::kViewportWidth, UCDD::kViewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2.ʹ�������ͼ��Ⱦ����
	m_debugDepthShader->use();
	m_debugDepthShader->setFloat("nearPlane", nearPlane);
	m_debugDepthShader->setFloat("farPlane", farPlane);

	m_quadVAO->bindVAO();
	m_depthMapFb->bindTexture();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
}

void AdvancedLightingState::drawScene(std::shared_ptr<AbstractShader> shader)
{
	shader->use();

	glm::mat4 modelMat = glm::mat4(1.0f);
	shader->setMatrix("modelMat", modelMat);

	m_woodFloorVAO->bindVAO();
	m_woodFloorVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(0.0f, 1.5f, 0.0));
	modelMat = glm::scale(modelMat, glm::vec3(0.5f));
	shader->setMatrix("modelMat", modelMat);

	drawCube();

	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(2.0f, 0.0f, 1.0));
	modelMat = glm::scale(modelMat, glm::vec3(0.5f));
	shader->setMatrix("modelMat", modelMat);
	drawCube();

	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(-1.0f, 0.0f, 2.0));
	modelMat = glm::rotate(modelMat, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	modelMat = glm::scale(modelMat, glm::vec3(0.25f));
	shader->setMatrix("modelMat", modelMat);
	drawCube();
}

void AdvancedLightingState::drawShadow()
{
	// light space mat
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

	float nearPlane = 1.0f;
	float farPlane = 7.5f;

	auto lightViewMat = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto lightProjMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

	m_depthMapShader->use();
	auto lightSpaceMat = lightProjMat * lightViewMat;
	m_depthMapShader->setMatrix("lightSpaceMat", lightSpaceMat);

	// 1.��Ⱦ�����ͼ���Ǵӹ��͸��ͼ����Ⱦ�������������������Ӱ
	// ��Ӱ��ͼ����������ԭ����Ⱦ�ĳ�����ͨ���Ǵ��ڷֱ��ʣ����Ų�ͬ�ķֱ��ʣ�������Ҫ�ı��ӿڣ�viewport���Ĳ�������Ӧ��Ӱ��ͼ�ĳߴ硣
	// ����������˸����ӿڲ��������������ͼҪô̫СҪô�Ͳ�������
	glViewport(0, 0, UCDD::kShadowWidth, UCDD::kShadowHeight);
	m_depthMapFb->bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT); // ���֡����������Ȼ���

	drawScene(m_depthMapShader);

	// reset viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, UCDD::kViewportWidth, UCDD::kViewportHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2.�ٴ���Ⱦ����
	m_shadowShader->use();
	m_shadowShader->setMatrix("lightSpaceMat", lightSpaceMat);
	m_shadowShader->setVec("viewPos", m_cameraWrapper->cameraPos());
	m_shadowShader->setVec("lightPos", lightPos);
	m_shadowShader->setBool("isBlinn", m_controlParam->isBlinn);
	m_shadowShader->setBool("isGammaCorrection", m_controlParam->isGammaCorrection);

	m_woodFloorVAO->bindTexture();
	m_depthMapFb->bindTexture(GL_TEXTURE1);
	drawScene(m_shadowShader);
}


