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
	initTransformMatUniformBlock();

	m_woodFloorVAO = Singleton<TriangleVAOFactory>::instance()->createFloorVAO("skin/textures/wood.png");
	m_cubeVAO = Singleton<TriangleVAOFactory>::instance()->createAdvancedTargetVAO();
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

	auto cameraWrapper = Singleton<CameraWrapper>::instance();
	cameraWrapper->setCursorVisible(false);

	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	auto viewMat = cameraWrapper->lookAtMatrix();

	// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
	auto projectionMat = glm::perspective(glm::radians(cameraWrapper->fieldOfView()), float(UCDD::kViewportWidth) / UCDD::kViewportHeight, 0.1f, 100.0f);
	
	m_shader->use();

	glBindBuffer(GL_UNIFORM_BUFFER, m_transformMatUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMat));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMat));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	switch (m_controlParam->stateKey) {
	case GLFW_KEY_1: {
		drawFloor();
		break;
	}
	case GLFW_KEY_2: {
		drawDepthMap();
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

		// 方法1, 使能gamma校正（丧失一些控制权）
		//if (m_controlParam->isGammaCorrection) {
		//	glEnable(GL_FRAMEBUFFER_SRGB);
		//}
		//else {
		//	glDisable(GL_FRAMEBUFFER_SRGB);
		//}

		// 方法2，每个片段着色器最终颜色输出时（缺点：每个着色器都要处理），获取输出颜色的 1/gamma 次幂
		m_shader->use();
		m_shader->setBool("isGammaCorrection", m_controlParam->isGammaCorrection);

		// 方法3，加入后期处理，只需一个片段着色器处理最终颜色输出
		// TODO

		// sRGB纹理（看到的图像实际以经过gamma校正，设计者实在sRGB空间设计纹理，而不是线性控件，二次校正导致光线太亮）
		// 解决方式使用sRGB纹理格式（自动把颜色校正线性空间中）
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

void AdvancedLightingState::drawCube(std::shared_ptr<AbstractShader> shader)
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
	m_depthMapShader->setMatrix("lightViewMat", glm::value_ptr(lightViewMat));
	m_depthMapShader->setMatrix("lightProjMat", glm::value_ptr(lightProjMat));

	// 1.渲染深度贴图，是从光的透视图里渲染的深度纹理，用它计算阴影
	// 阴影贴图经常和我们原来渲染的场景（通常是窗口分辨率）有着不同的分辨率，我们需要改变视口（viewport）的参数以适应阴影贴图的尺寸。
	// 如果我们忘了更新视口参数，最后的深度贴图要么太小要么就不完整。
	glViewport(0, 0, UCDD::kShadowWidth, UCDD::kShadowHeight);
	m_depthMapFb->bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT); // 深度帧缓冲清理深度缓冲

	drawFloor();

	m_depthMapShader->use();

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(0.0f, 1.5f, 0.0));
	modelMat = glm::scale(modelMat, glm::vec3(0.5f));
	m_depthMapShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	drawCube(m_depthMapShader);

	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(2.0f, 0.0f, 1.0));
	modelMat = glm::scale(modelMat, glm::vec3(0.5f));
	m_depthMapShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	drawCube(m_depthMapShader);

	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(-1.0f, 0.0f, 2.0));
	modelMat = glm::rotate(modelMat, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	modelMat = glm::scale(modelMat, glm::vec3(0.25f));
	m_depthMapShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	drawCube(m_depthMapShader);
	
	// reset viewport
	glViewport(0, 0, UCDD::kViewportWidth, UCDD::kViewportHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2.使用深度贴图渲染场景
	m_debugDepthShader->use();
	m_debugDepthShader->setFloat("nearPlane", nearPlane);
	m_debugDepthShader->setFloat("farPlane", farPlane);

	m_quadVAO->bindVAO();
	m_depthMapFb->bindTexture();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
}


