#include "PbrExerciceState.h"

#include "Singleton.h"
#include "CommonDataDef.h"
#include "TextureHelper.h"

extern GLFWwindow* g_globalWindow;

namespace UCDD = UiCommonDataDef;

PbrExerciceState::PbrExerciceState() 
	: AbstractExerciceState()
{
	m_cameraWrapper = Singleton<CameraWrapper>::instance();
	m_shaderFactory = Singleton<ShaderFactory>::instance();

	initTransformMatUniformBlock();

	initLighting();
	initLightingTexture();
}

void PbrExerciceState::render()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	m_cameraWrapper->setCursorVisible(false);

	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	auto viewMat = m_cameraWrapper->lookAtMatrix();

	// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
	auto projectionMat = glm::perspective(glm::radians(m_cameraWrapper->fieldOfView()), float(UCDD::kViewportWidth) / UCDD::kViewportHeight, 0.1f, 100.0f);

	glBindBuffer(GL_UNIFORM_BUFFER, m_transformMatUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMat));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMat));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	switch (m_stateKey) {
	case GLFW_KEY_1: {
		drawPbrLighting();
		break;
	case GLFW_KEY_2: {
		drawPbrLightingTexture();
		break;
	}
	default:
		break;
	}
	}
}

void PbrExerciceState::initLighting()
{
	m_sphereVAO = Singleton<TriangleVAOFactory>::instance()->createPbrLightingVAO();

	m_pbrLightShader = m_shaderFactory->shaderProgram("pbr_light", "ShaderProgram/Pbr/pbr_light.vs", "ShaderProgram/Pbr/pbr_light.fs");
	m_pbrLightShader->use();
	m_pbrLightShader->setBool("reverseNormals", false);
	m_pbrLightShader->setVec("albedo", 0.5f, 0.0f, 0.0f);
	m_pbrLightShader->setFloat("ao", 1.0f);

	// lights
	m_lightPositions = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	m_lightColors = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
}

void PbrExerciceState::initLightingTexture()
{
	m_pbrLightTexShader = m_shaderFactory->shaderProgram("pbr_light_tex", "ShaderProgram/Pbr/pbr_light.vs", "ShaderProgram/Pbr/pbr_texture_light.fs");
	m_pbrLightTexShader->use();
	m_pbrLightTexShader->setBool("reverseNormals", false);

	m_pbrLightTexShader->setInt("albedoMap", 0);
	m_pbrLightTexShader->setInt("normalMap", 1);
	m_pbrLightTexShader->setInt("metallicMap", 2);
	m_pbrLightTexShader->setInt("roughnessMap", 3);
	m_pbrLightTexShader->setInt("aoMap", 4);

	m_sphereAlbedoTexId = TextureHelper::loadTexture("skin/textures/pbr/rusted_iron/albedo.png");
	m_sphereNormalTexId = TextureHelper::loadTexture("skin/textures/pbr/rusted_iron/normal.png");
	m_sphereMetallicTexId = TextureHelper::loadTexture("skin/textures/pbr/rusted_iron/metallic.png");
	m_sphereRoughnessTexId = TextureHelper::loadTexture("skin/textures/pbr/rusted_iron/roughness.png");
	m_sphereAoTexId = TextureHelper::loadTexture("skin/textures/pbr/rusted_iron/ao.png");
}

void PbrExerciceState::drawSphere()
{
	m_sphereVAO->bindVAO();

	glDrawElements(GL_TRIANGLE_STRIP, m_sphereVAO->indexCount(), GL_UNSIGNED_INT, 0);
}

void PbrExerciceState::drawPbrLighting()
{
	m_pbrLightShader->use();
	m_pbrLightShader->setVec("camPos", m_cameraWrapper->cameraPos());

	// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
	int rows = 7;
	int cols = 7;
	float spacing = 2.5;

	// 从下往上金属性0.0~1.0，从左往右粗糙度0.0~1.0
	// -7.5 ~ 7.5
	glm::mat4 model = glm::mat4(1.0f);
	for (int row = 0; row < rows; ++row) {
		m_pbrLightShader->setFloat("metallic", (float)row / (float)rows);
		for (int col = 0; col < cols; ++col) {
			// we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
			// on direct lighting.
			m_pbrLightShader->setFloat("roughness", glm::clamp((float)col / (float)cols, 0.05f, 1.0f));

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				(col - (cols / 2)) * spacing, 
				(row - (rows / 2)) * spacing,
				0.0f
			));
			m_pbrLightShader->setMatrix("modelMat", model);
			m_pbrLightShader->setMatrix("normalMat", glm::transpose(glm::inverse(glm::mat3(model))));
			drawSphere();
		}
	}

	// render light source (simply re-render sphere at light positions)
	// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
	// keeps the codeprint small.
	unsigned int count = static_cast<unsigned int>(sizeof(glm::vec3) * m_lightPositions.size() / sizeof(m_lightPositions[0]));
	for (unsigned int i = 0; i < count; ++i) {
		glm::vec3 newPos = m_lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);// + -25~25
		newPos = m_lightPositions[i];
		m_pbrLightShader->setVec("lightPositions[" + std::to_string(i) + "]", newPos);
		m_pbrLightShader->setVec("lightColors[" + std::to_string(i) + "]", m_lightColors[i]);

		model = glm::mat4(1.0f);
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.5f));
		m_pbrLightShader->setMatrix("modelMat", model);
		m_pbrLightShader->setMatrix("normalMat", glm::transpose(glm::inverse(glm::mat3(model))));
		drawSphere();
	}
}

void PbrExerciceState::drawPbrLightingTexture()
{
	m_pbrLightTexShader->use();
	m_pbrLightTexShader->setVec("camPos", m_cameraWrapper->cameraPos());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_sphereAlbedoTexId);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_sphereNormalTexId);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_sphereMetallicTexId);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_sphereRoughnessTexId);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_sphereAoTexId);

	// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
	int rows = 7;
	int cols = 7;
	float spacing = 2.5;

	// -7.5 ~ 7.5
	glm::mat4 model = glm::mat4(1.0f);
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				(col - (cols / 2)) * spacing,
				(row - (rows / 2)) * spacing,
				0.0f
			));
			m_pbrLightTexShader->setMatrix("modelMat", model);
			m_pbrLightTexShader->setMatrix("normalMat", glm::transpose(glm::inverse(glm::mat3(model))));
			drawSphere();
		}
	}

	// render light source (simply re-render sphere at light positions)
	// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
	// keeps the codeprint small.
	unsigned int count = static_cast<unsigned int>(sizeof(glm::vec3) * m_lightPositions.size() / sizeof(m_lightPositions[0]));
	for (unsigned int i = 0; i < count; ++i) {
		glm::vec3 newPos = m_lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);// + -25~25
		newPos = m_lightPositions[i];
		m_pbrLightTexShader->setVec("lightPositions[" + std::to_string(i) + "]", newPos);
		m_pbrLightTexShader->setVec("lightColors[" + std::to_string(i) + "]", m_lightColors[i]);

		model = glm::mat4(1.0f);
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.5f));
		m_pbrLightTexShader->setMatrix("modelMat", model);
		m_pbrLightTexShader->setMatrix("normalMat", glm::transpose(glm::inverse(glm::mat3(model))));
		drawSphere();
	}
}
