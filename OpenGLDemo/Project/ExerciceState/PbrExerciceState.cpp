#include "PbrExerciceState.h"

#include "Singleton.h"
#include "CommonDataDef.h"
#include "TextureHelper.h"

extern GLFWwindow* g_globalWindow;

namespace UCDD = UiCommonDataDef;

PbrExerciceState::PbrExerciceState() 
	: AbstractExerciceState()
	, m_rows(7)
	, m_cols(7)
	, m_spacing(2.5f)
{
	m_cameraWrapper = Singleton<CameraWrapper>::instance();
	m_shaderFactory = Singleton<ShaderFactory>::instance();

	initTransformMatUniformBlock();

	initLighting();
	initLightingTexture();
	initIblIrradianceConversion();
}

PbrExerciceState::~PbrExerciceState()
{
	glDeleteTextures(1, &m_sphereAlbedoTexId);
	glDeleteTextures(1, &m_sphereNormalTexId);
	glDeleteTextures(1, &m_sphereMetallicTexId);
	glDeleteTextures(1, &m_sphereRoughnessTexId);
	glDeleteTextures(1, &m_sphereAoTexId);

	glDeleteTextures(1, &m_equiRectMapTexId);
	glDeleteTextures(1, &m_envCubemapTexId);
	glDeleteTextures(1, &m_irradianceMapTexId);
}

void PbrExerciceState::render()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.

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
		drawPbrLighting(m_pbrLightShader);
		break;
	}
	case GLFW_KEY_2: {
		drawPbrLightingTexture();
		break;
	}
	case GLFW_KEY_3: {
		drawIblIrradianceConversion();
		break;
	}
	case GLFW_KEY_4: {
		drawPbrLightingConvolution();
		break;
	}
	default:
		break;
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

void PbrExerciceState::initIblIrradianceConversion()
{
	m_cubeVAO = Singleton<TriangleVAOFactory>::instance()->createNormalCubeVAO();

	m_equiRectToCubemapShader = m_shaderFactory->shaderProgram("equi_rect_to_cubemap", "ShaderProgram/Pbr/cubemap.vs", "ShaderProgram/Pbr/equirectangular_to_cubemap.fs");
	m_equiRectToCubemapShader->use();
	m_equiRectToCubemapShader->setInt("equirectangularMap", 0);	
	
	auto projectionMat = glm::perspective(glm::radians(m_cameraWrapper->fieldOfView()), float(UCDD::kViewportWidth) / UCDD::kViewportHeight, 0.1f, 100.0f);

	m_irradianceConvolutionShader = m_shaderFactory->shaderProgram("irradiance_convolution_shader", "ShaderProgram/Pbr/cubemap.vs", "ShaderProgram/Pbr/irradiance_convolution.fs");
	m_irradianceConvolutionShader->use();
	m_irradianceConvolutionShader->setInt("environmentMap", 0);

	m_backgroundShader = m_shaderFactory->shaderProgram("pbr_background", "ShaderProgram/Pbr/background.vs", "ShaderProgram/Pbr/background.fs");
	m_backgroundShader->use();
	m_backgroundShader->setInt("environmentMap", 0);
	m_backgroundShader->setMatrix("projection", projectionMat);

	m_pbrLightConvolutionShader = m_shaderFactory->shaderProgram("pbr_light_conbolution", "ShaderProgram/Pbr/pbr_light.vs", "ShaderProgram/Pbr/pbr_light irraiance.fs");
	m_pbrLightConvolutionShader->use();
	m_pbrLightConvolutionShader->setBool("reverseNormals", false);
	m_pbrLightConvolutionShader->setVec("albedo", 0.5f, 0.0f, 0.0f);
	m_pbrLightConvolutionShader->setFloat("ao", 1.0f);
	m_pbrLightConvolutionShader->setMatrix("projection", projectionMat);

	drawEquiRectMapToCubemap();
	drawIblIrradianceMap();

	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(g_globalWindow, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);
}

void PbrExerciceState::drawSphere()
{
	m_sphereVAO->bindVAO();

	glDrawElements(GL_TRIANGLE_STRIP, m_sphereVAO->indexCount(), GL_UNSIGNED_INT, 0);
}

void PbrExerciceState::drawCube()
{
	m_cubeVAO->bindVAO();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void PbrExerciceState::drawSkybox()
{
	// render skybox (render as last to prevent overdraw)
	m_backgroundShader->use();
	m_backgroundShader->setMatrix("view", m_cameraWrapper->lookAtMatrix());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemapTexId);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMapTexId);
	drawCube();
}

void PbrExerciceState::drawPbrLighting(std::shared_ptr<AbstractShader> shader)
{
	shader->use();
	shader->setVec("camPos", m_cameraWrapper->cameraPos());

	// bind pre-computed IBL data
	if (shader == m_pbrLightConvolutionShader) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMapTexId);
	}

	// render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
	// 从下往上金属性0.0~1.0，从左往右粗糙度0.0~1.0
	// -7.5 ~ 7.5
	glm::mat4 model = glm::mat4(1.0f);
	for (int row = 0; row < m_rows; ++row) {
		shader->setFloat("metallic", (float)row / (float)m_rows);
		for (int col = 0; col < m_cols; ++col) {
			// we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
			// on direct lighting.
			shader->setFloat("roughness", glm::clamp((float)col / (float)m_cols, 0.05f, 1.0f));

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				(col - (m_cols / 2)) * m_spacing,
				(row - (m_rows / 2)) * m_spacing,
				0.0f
			));
			shader->setMatrix("modelMat", model);
			shader->setMatrix("normalMat", glm::transpose(glm::inverse(glm::mat3(model))));
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
		shader->setVec("lightPositions[" + std::to_string(i) + "]", newPos);
		shader->setVec("lightColors[" + std::to_string(i) + "]", m_lightColors[i]);

		model = glm::mat4(1.0f);
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.5f));
		shader->setMatrix("modelMat", model);
		shader->setMatrix("normalMat", glm::transpose(glm::inverse(glm::mat3(model))));
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
	// -7.5 ~ 7.5
	glm::mat4 model = glm::mat4(1.0f);
	for (int row = 0; row < m_rows; ++row) {
		for (int col = 0; col < m_cols; ++col) {
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				(col - (m_cols / 2)) * m_spacing,
				(row - (m_rows / 2)) * m_spacing,
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

void PbrExerciceState::drawIblIrradianceConversion()
{
	drawPbrLighting(m_pbrLightShader);
	drawSkybox();
}

void PbrExerciceState::drawEquiRectMapToCubemap()
{
	m_captureFb = FramebufferFactory::createFramebuffer();

	// pbr: load the HDR environment map
	TextureHelper::TexParam texParam;
	texParam.internalFormat3 = GL_RGB16F;
	texParam.format = GL_RGB;
	texParam.type = GL_FLOAT;
	m_equiRectMapTexId = TextureHelper::loadTexture("skin/textures/hdr/newport_loft.hdr", texParam);

	texParam.width = 512;
	texParam.height = 512;
	texParam.internalFormat3 = GL_RGB16F;
	texParam.format = GL_RGB;
	texParam.type = GL_FLOAT;
	m_envCubemapTexId = TextureHelper::loadCubemap(std::vector<std::string>(), texParam);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	m_captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	m_captureViews =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	m_equiRectToCubemapShader->use();
	m_equiRectToCubemapShader->setMatrix("projection", m_captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_equiRectMapTexId);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	m_captureFb->bindFramebuffer();
	for (unsigned int i = 0; i < 6; ++i) {
		m_equiRectToCubemapShader->setMatrix("view", m_captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envCubemapTexId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PbrExerciceState::drawIblIrradianceMap()
{
	// 由于辐照度图对所有周围的辐射值取了平均值，因此它丢失了大部分高频细节，所以我们可以以较低的分辨率（32x32）存储，
	// 并让 OpenGL 的线性滤波完成大部分工作
	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// pbr: load the HDR environment map
	TextureHelper::TexParam texParam;
	texParam.internalFormat3 = GL_RGB16F;
	texParam.format = GL_RGB;
	texParam.type = GL_FLOAT;
	texParam.width = 32;
	texParam.height = 32;
	m_irradianceMapTexId = TextureHelper::loadCubemap(std::vector<std::string>(), texParam);
	m_captureFb->bindFramebuffer();
	m_captureFb->renderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 32, 32);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	m_irradianceConvolutionShader->use();
	m_irradianceConvolutionShader->setMatrix("projection", m_captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemapTexId);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	m_captureFb->bindFramebuffer();
	for (unsigned int i = 0; i < 6; ++i) {
		m_irradianceConvolutionShader->setMatrix("view", m_captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradianceMapTexId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PbrExerciceState::drawPbrLightingConvolution()
{
	drawPbrLighting(m_pbrLightConvolutionShader);
	drawSkybox();
}

