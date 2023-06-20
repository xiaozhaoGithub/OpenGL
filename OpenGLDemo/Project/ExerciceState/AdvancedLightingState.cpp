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

	drawFloor();
}

void AdvancedLightingState::processInput()
{
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


