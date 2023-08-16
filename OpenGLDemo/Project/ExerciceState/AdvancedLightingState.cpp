#include "AdvancedLightingState.h"

#include <vector>
#include <random>

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
	bool isShowShadow = false;
	float heightScale = 0.1f;
	float exposure = 1.0f;
	bool isUseHdr = false;
	bool isBloom = true;
};

AdvancedLightingState::AdvancedLightingState()
	: m_controlParam(std::unique_ptr<AdvancedLightingStateControlParam>(new AdvancedLightingStateControlParam()))
{
	m_cameraWrapper = Singleton<CameraWrapper>::instance();

	initTransformMatUniformBlock();

	m_woodFloorVAO = Singleton<TriangleVAOFactory>::instance()->createFloorVAO("skin/textures/wood.png");
	m_cubeVAO = Singleton<TriangleVAOFactory>::instance()->createTexCubeVAO();
	m_quadVAO = Singleton<TriangleVAOFactory>::instance()->createQuadVAO();
	m_normalMapVAO = Singleton<TriangleVAOFactory>::instance()->createNormalMapVAO();
	m_parallaxMapVAO = Singleton<TriangleVAOFactory>::instance()->createParallaxMapVAO();

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

	m_depthCubeMapShader = shaderFactory->shaderProgram("depth_cube_map", "ShaderProgram/AdvancedLight/omnidirection_depth_map.vs", "ShaderProgram/AdvancedLight/omnidirection_depth_map.gs", "ShaderProgram/AdvancedLight/omnidirection_depth_map.fs");
	m_omnidirectionShadowShader = shaderFactory->shaderProgram("omnidirection_shadow", "ShaderProgram/AdvancedLight/omnidirection_shadow.vs", "ShaderProgram/AdvancedLight/omnidirection_shadow.fs");
	m_omnidirectionShadowShader->use();
	m_omnidirectionShadowShader->setInt("diffuseTexture", 0);
	m_omnidirectionShadowShader->setInt("shadowCubeMap", 1);

	m_normalMapShader = shaderFactory->shaderProgram("normal_map", "ShaderProgram/AdvancedLight/normal_map.vs", "ShaderProgram/AdvancedLight/normal_map.fs");
	m_normalMapShader->use();
	m_normalMapShader->setInt("diffuseMap", 0);
	m_normalMapShader->setInt("normalMap", 1);

	m_parallaxMapShader = shaderFactory->shaderProgram("parallax_map", "ShaderProgram/AdvancedLight/normal_map.vs", "ShaderProgram/AdvancedLight/parallax_map.fs");
	m_parallaxMapShader->use();
	m_parallaxMapShader->setInt("diffuseMap", 0);
	m_parallaxMapShader->setInt("normalMap", 1);
	m_parallaxMapShader->setInt("depthMap", 2);

	m_hdrLightShader = shaderFactory->shaderProgram("hdr_light", "ShaderProgram/AdvancedLight/omnidirection_shadow.vs", "ShaderProgram/AdvancedLight/hdr_light.fs");
	m_hdrLightShader->use();
	m_hdrLightShader->setInt("diffuseTexture", 0);

	m_hdrShader = shaderFactory->shaderProgram("hdr_quad", "ShaderProgram/Advanced/screen_texture_shader.vs", "ShaderProgram/AdvancedLight/hdr_quad.fs");
	m_hdrShader->use();
	m_hdrShader->setInt("hdrFbTexture", 0);

	m_bloomShader = shaderFactory->shaderProgram("bloom", "ShaderProgram/AdvancedLight/omnidirection_shadow.vs", "ShaderProgram/AdvancedLight/bloom.fs");
	m_bloomShader->use();
	m_bloomShader->setInt("diffuseTexture", 0);

	m_lightBoxShader = shaderFactory->shaderProgram("light_box", "ShaderProgram/AdvancedLight/omnidirection_shadow.vs", "ShaderProgram/AdvancedLight/light_box.fs");

	m_blurShader = shaderFactory->shaderProgram("blur", "ShaderProgram/Advanced/screen_texture_shader.vs", "ShaderProgram/AdvancedLight/blur.fs");
	m_blurShader->use();
	m_blurShader->setInt("brightTexture", 0);

	m_bloomFinalShader = shaderFactory->shaderProgram("bloom_final", "ShaderProgram/Advanced/screen_texture_shader.vs", "ShaderProgram/AdvancedLight/bloom_final.fs");
	m_bloomFinalShader->use();
	m_bloomFinalShader->setInt("scene", 0);
	m_bloomFinalShader->setInt("bloomBlur", 1);

	m_depthMapFb = FramebufferFactory::createDepthFb();
	m_cubeMapDepthFb = FramebufferFactory::createCubeMapDepthFb();

	FramebufferFactory::FramebufferParam fbParam;
	fbParam.internalFormat = GL_RGB16F;
	m_floatFb = FramebufferFactory::createFramebuffer(fbParam);

	fbParam.internalFormat = GL_RGBA16F;
	m_multAttachFloatFb = FramebufferFactory::createFramebuffer(fbParam, 2);
	m_pingpongFbs = { FramebufferFactory::createFramebuffer(fbParam), FramebufferFactory::createFramebuffer(fbParam) };
	
	m_woodTexId = TextureHelper::loadTexture("skin/textures/wood.png");
	m_containerTexId = TextureHelper::loadTexture("skin/container2.png");

	initDefferedShading();
	initSsaoShading();

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
	case GLFW_KEY_4: {
		drawOmnidirectionShadow();
		break;
	}	
	case GLFW_KEY_5: {
		drawNormalMap();
		break;
	}
	case GLFW_KEY_6: {
		drawParallaxMap();
		break;
	}
	case GLFW_KEY_7: {
		drawHdr();
		break;
	}
	case GLFW_KEY_8: {
		drawBloom();
		break;
	}
	case GLFW_KEY_9: {
		drawDeferredShading();
		break;
	}	
	case GLFW_KEY_0: {
		drawSsao();
		break;
	}
	default:
		break;
	}
}

void AdvancedLightingState::processInput()
{
	for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; key++) {
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

	if (glfwGetKey(g_globalWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
		m_controlParam->isShowShadow = !m_controlParam->isShowShadow;
		m_controlParam->isUseHdr = !m_controlParam->isUseHdr;
		m_controlParam->isBloom = !m_controlParam->isBloom;
	}

	if (glfwGetKey(g_globalWindow, GLFW_KEY_Q) == GLFW_PRESS) {
		if (m_controlParam->stateKey == GLFW_KEY_6) {
			m_controlParam->heightScale = m_controlParam->heightScale > 0.0f ? m_controlParam->heightScale - 0.0005f : 0.0f;
		}

		m_controlParam->exposure = m_controlParam->exposure > 0.0f ? m_controlParam->exposure - 0.001f : 0.0f;
	}

	if (glfwGetKey(g_globalWindow, GLFW_KEY_E) == GLFW_PRESS) {
		if (m_controlParam->stateKey == GLFW_KEY_6) {
			m_controlParam->heightScale = m_controlParam->heightScale < 1.0f ? m_controlParam->heightScale + 0.0005f : 1.0f;
		}

		m_controlParam->exposure += 0.001f;
	}
}

void AdvancedLightingState::initDefferedShading()
{
	// deffered shading
	m_geometryPassShader = Singleton<ShaderFactory>::instance()->shaderProgram("g_buffer", "ShaderProgram/AdvancedLight/omnidirection_shadow.vs", "ShaderProgram/AdvancedLight/g_buffer.fs");
	m_lightingPassShader = Singleton<ShaderFactory>::instance()->shaderProgram("deffered_shading", "ShaderProgram/Advanced/screen_texture_shader.vs", "ShaderProgram/AdvancedLight/deffered_shading.fs");
	m_backpackModel = std::make_unique<Model>("skin/nanosuit/nanosuit.obj");

	m_backpackObjectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
	m_backpackObjectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
	m_backpackObjectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
	m_backpackObjectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
	m_backpackObjectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
	m_backpackObjectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
	m_backpackObjectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
	m_backpackObjectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
	m_backpackObjectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));

	// configure g-buffer fb
	FramebufferFactory::FramebufferParam fbParam;
	fbParam.internalFormat = GL_RGBA16F;
	// attach 3�������ʵ�ʿ�ֱ����GL_RGBA���˴��ӿ�δʵ�֣����Ҷ��ø��㾫�ȴ洢����
	m_defferedShadingFloatFb = FramebufferFactory::createFramebuffer(fbParam, 3);

	// lighting info
	const unsigned int lightSourceNum = 32;
	srand(13);
	for (unsigned int i = 0; i < lightSourceNum; i++) {
		// calculate slightly random offsets
		float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
		float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
		float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
		m_lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

		// also calculate random color
		float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		m_lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	// shader configuration
	m_lightingPassShader->use();
	m_lightingPassShader->setInt("gPosition", 0);
	m_lightingPassShader->setInt("gNormal", 1);
	m_lightingPassShader->setInt("gAlbedoSpec", 2);
}

void AdvancedLightingState::initSsaoShading()
{
	// shader configuration
	m_ssaoGeometryPassShader = Singleton<ShaderFactory>::instance()->shaderProgram("ssao_geometry", "ShaderProgram/AdvancedLight/ssao_geometry.vs", "ShaderProgram/AdvancedLight/ssao_geometry.fs");
	
	m_ssaoShader = Singleton<ShaderFactory>::instance()->shaderProgram("ssao", "ShaderProgram/Advanced/screen_texture_shader.vs", "ShaderProgram/AdvancedLight/ssao.fs");
	m_ssaoShader->use();
	m_ssaoShader->setInt("gPosition", 0);
	m_ssaoShader->setInt("gNormal", 1);
	m_ssaoShader->setInt("gTexNoise", 2);
	// tile noise texture over screen based on screen dimensions divided by noise size
	m_ssaoShader->setVec("noiseScale", glm::vec2(UCDD::kViewportWidth / 4.0f, UCDD::kViewportHeight / 4.0f));

	m_ssaoBlurShader = Singleton<ShaderFactory>::instance()->shaderProgram("ssao_blur", "ShaderProgram/Advanced/screen_texture_shader.vs", "ShaderProgram/AdvancedLight/ssao_blur.fs");
	m_ssaoBlurShader->use();
	m_ssaoBlurShader->setInt("gSsao", 0);

	m_ssaoLightingPassShader = Singleton<ShaderFactory>::instance()->shaderProgram("ssao_lighting", "ShaderProgram/Advanced/screen_texture_shader.vs", "ShaderProgram/AdvancedLight/ssao_lighting.fs");
	m_ssaoLightingPassShader->use();
	m_ssaoLightingPassShader->setInt("gPosition", 0);
	m_ssaoLightingPassShader->setInt("gNormal", 1);
	m_ssaoLightingPassShader->setInt("gAlbedo", 2);
	m_ssaoLightingPassShader->setInt("gSsao", 3);


	// configure g-buffer fb
	FramebufferFactory::FramebufferParam fbParam;
	fbParam.internalFormat = GL_RGBA16F;
	m_ssaoGeometryFloatFb = FramebufferFactory::createFramebuffer(fbParam, 3);

	// also create framebuffer to hold SSAO processing stage
	fbParam.internalFormat = GL_RED;
	fbParam.format = GL_RED;
	m_ssaoFloatFb = FramebufferFactory::createFramebuffer(fbParam);

	// blur ssao
	m_ssaoBlurFloatFb = FramebufferFactory::createFramebuffer(fbParam);

	// �����������
	auto myLerp = [](float a, float b, float f) {
		return a + f * (b - a);
	};

	std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;
	for (int i = 0; i < 64; ++i) {
		glm::vec3 sample(glm::vec3(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator)));
		sample = normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0f;

		// �������ע����ڿ�������Ƭ�ε��ڱ��ϣ����ӽ������������ĵ�������ʹ�ü��ٲ�ֵ����
		// ���ž���
		scale = myLerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		m_ssaoKernel.emplace_back(sample);
	}

	// generate noise texture
	// �Գ�����ÿһ��Ƭ�δ���һ�������ת�����������ܿ콫�ڴ�ľ������ԣ����õķ����Ǵ���һ��С�������ת��������ƽ������Ļ�ϡ�
	// �����������ת��
	std::vector<glm::vec3> ssaoNoise;
	for (int i = 0; i < 64; ++i) {
		// ��������Χ��Z����ת
		glm::vec3 noise(glm::vec3(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0));
		ssaoNoise.emplace_back(noise);
	}

	TextureHelper::TexParam noiseTexParam;
	noiseTexParam.internalFormat3 = GL_RGBA32F;
	noiseTexParam.format = GL_RGB;
	noiseTexParam.width = 4;
	noiseTexParam.height = 4;
	noiseTexParam.data = &ssaoNoise[0];
	m_ssaoNoiseTexId = TextureHelper::loadTexture(noiseTexParam);


	// lighting info
	m_lightPos = glm::vec3(2.0, 4.0, -2.0);
	//m_lightColor = glm::vec3(1.0, 1.0, 1.0);
	m_lightColor = glm::vec3(0.2, 0.2, 0.7);
}

void AdvancedLightingState::drawQuad()
{
	m_quadVAO->bindVAO();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
	glBindVertexArray(0);
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

void AdvancedLightingState::drawCube(unsigned int texId)
{
	m_cubeVAO->bindVAO();
	if (texId == 0) {
		m_cubeVAO->bindTexture();
	} 
	else {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texId);
	}

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

void AdvancedLightingState::drawSceneToDepthMap(std::shared_ptr<AbstractShader> shader)
{
	// light space mat
	glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

	float nearPlane = 1.0f;
	float farPlane = 7.5f;

	auto lightViewMat = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto lightProjMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

	shader->use();
	shader->setMatrix("lightSpaceMat", lightProjMat * lightViewMat);

	// 1.��Ⱦ�����ͼ���Ǵӹ��͸��ͼ����Ⱦ�������������������Ӱ
	// ��Ӱ��ͼ����������ԭ����Ⱦ�ĳ�����ͨ���Ǵ��ڷֱ��ʣ����Ų�ͬ�ķֱ��ʣ�������Ҫ�ı��ӿڣ�viewport���Ĳ�������Ӧ��Ӱ��ͼ�ĳߴ硣
	// ����������˸����ӿڲ��������������ͼҪô̫СҪô�Ͳ�������
	glViewport(0, 0, UCDD::kShadowWidth, UCDD::kShadowHeight);
	m_depthMapFb->bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT); // ���֡����������Ȼ���

	glm::mat4 modelMat = glm::mat4(1.0f);
	shader->setMatrix("modelMat", modelMat);

	m_woodFloorVAO->bindVAO();
	m_woodFloorVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// ʹ�������޳�������ʹ�����ͼ�����ֵ������������ֵ����󣬴󵽱�Ե�㣬ȷ����Ӱ��׼ȷ
	// �ӽ���Ӱ��������Ȼ����ֲ���ȷ��Ч�������뿼�ǵ���ʱʹ�������޳�������������塣����ʹ����ͨ��ƫ��ֵͨ�����ܱ���peter panning��
	// ��ʹ�ú��ʵļ���ƫ�Ʒ�ʽ���˴��Ȳ�ʹ�������޳�����Ӱ�����彻�㴦�Դ������⣩
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

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

	//glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
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
	// auto lightProjMat = glm::perspective(glm::radians(45.0f), float(UCDD::kViewportWidth) / UCDD::kViewportHeight, nearPlane, farPlane);
	auto lightSpaceMat = lightProjMat * lightViewMat;

	// 1.��Ⱦ�����ͼ���Ǵӹ��͸��ͼ����Ⱦ�������������������Ӱ
	// ��Ӱ��ͼ����������ԭ����Ⱦ�ĳ�����ͨ���Ǵ��ڷֱ��ʣ����Ų�ͬ�ķֱ��ʣ�������Ҫ�ı��ӿڣ�viewport���Ĳ�������Ӧ��Ӱ��ͼ�ĳߴ硣
	// ����������˸����ӿڲ��������������ͼҪô̫СҪô�Ͳ�������
	glViewport(0, 0, UCDD::kShadowWidth, UCDD::kShadowHeight);
	m_depthMapFb->bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT); // ���֡����������Ȼ���

	drawSceneToDepthMap(m_depthMapShader);

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

	m_woodFloorVAO->bindTexture();
	m_depthMapFb->bindTexture(GL_TEXTURE1);
	drawScene(m_shadowShader);
}

void AdvancedLightingState::drawOmnidirectionShadow()
{
	float near = 1.0f;
	float far = 25.0f;

	// 90�����ǲ��ܱ�֤��Ұ�㹻�󵽿��Ժ��ʵ�������������ͼ��һ����
	glm::mat4 projMat = glm::perspective(glm::radians(90.0f), (float)UCDD::kShadowWidth / UCDD::kShadowHeight, near, far);

	glm::vec3 lightPos(0.0f);
	std::vector<glm::mat4> shadowTransformMats = {
		projMat * glm::lookAt(lightPos, lightPos + glm::vec3(1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)),
		projMat * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)),
		projMat * glm::lookAt(lightPos, lightPos + glm::vec3(0.0,1.0,0.0), glm::vec3(0.0,0.0,1.0)),
		projMat * glm::lookAt(lightPos, lightPos + glm::vec3(0.0,-1.0,0.0), glm::vec3(0.0,0.0,-1.0)),
		projMat * glm::lookAt(lightPos, lightPos + glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,-1.0,0.0)),
		projMat * glm::lookAt(lightPos, lightPos + glm::vec3(0.0,0.0,-1.0), glm::vec3(0.0,-1.0,0.0))
	};

	// render cube map depth buffer
	glViewport(0, 0, UCDD::kShadowWidth, UCDD::kShadowHeight);
	m_cubeMapDepthFb->bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT); 

	m_depthCubeMapShader->use();
	m_depthCubeMapShader->setFloat("farPlane", far);
	m_depthCubeMapShader->setVec("lightPos", lightPos);
	for (int i = 0; i < 6; i++) {
		m_depthCubeMapShader->setMatrix("shadowTransformMats[" + std::to_string(i) + "]", shadowTransformMats[i]);
	}
	drawOmnidirectionShadowScene(m_depthCubeMapShader);

	// render scene
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, UCDD::kViewportWidth, UCDD::kViewportHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_omnidirectionShadowShader->use();
	m_omnidirectionShadowShader->setVec("viewPos", m_cameraWrapper->cameraPos());
	m_omnidirectionShadowShader->setVec("lightPos", lightPos);
	m_omnidirectionShadowShader->setFloat("farPlane", far);
	m_omnidirectionShadowShader->setBool("isBlinn", m_controlParam->isBlinn);
	m_omnidirectionShadowShader->setBool("isShowShadow", m_controlParam->isShowShadow);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_woodTexId);
	m_cubeMapDepthFb->bindTexture(GL_TEXTURE_CUBE_MAP, GL_TEXTURE1);
	drawOmnidirectionShadowScene(m_omnidirectionShadowShader);
}

void AdvancedLightingState::drawOmnidirectionShadowScene(std::shared_ptr<AbstractShader> shader)
{
	// room cube
	glm::mat4 model(1.0f);
	model = glm::scale(model, glm::vec3(5.0f));
	shader->setMatrix("modelMat", model);
	glDisable(GL_CULL_FACE); // Note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
	shader->setBool("reverseNormals", true); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
	drawCube();
	shader->setBool("reverseNormals", false);// And of course disable it
	glEnable(GL_CULL_FACE);

	// cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader->setMatrix("modelMat", model);
	drawCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.75f));
	shader->setMatrix("modelMat", model);
	drawCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader->setMatrix("modelMat", model);
	drawCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
	model = glm::scale(model, glm::vec3(0.5f));
	shader->setMatrix("modelMat", model);
	drawCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.75f));
	drawCube();
}

void AdvancedLightingState::drawNormalMap()
{
	glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

	m_normalMapShader->use();
	m_normalMapShader->setVec("lightPos", lightPos);
	m_normalMapShader->setVec("viewPos", m_cameraWrapper->cameraPos());

	glm::mat4 modelMat(1.0f);
	modelMat = glm::rotate(modelMat, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
	m_normalMapShader->setMatrix("modelMat", modelMat);

	m_normalMapVAO->bindVAO();
	m_normalMapVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);


	// light source
	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, lightPos);
	modelMat = glm::scale(modelMat, glm::vec3(0.1f));
	m_normalMapShader->setMatrix("modelMat", modelMat);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void AdvancedLightingState::drawParallaxMap()
{
	glm::vec3 lightPos(0.5f, 1.0f, 0.3f);

	m_parallaxMapShader->use();
	m_parallaxMapShader->setVec("lightPos", lightPos);
	m_parallaxMapShader->setVec("viewPos", m_cameraWrapper->cameraPos());
	m_parallaxMapShader->setFloat("heightScale", m_controlParam->heightScale);

	glm::mat4 modelMat(1.0f);
	//modelMat = glm::rotate(modelMat, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
	m_parallaxMapShader->setMatrix("modelMat", modelMat);

	m_parallaxMapVAO->bindVAO();
	m_parallaxMapVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// light source
	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, lightPos);
	modelMat = glm::scale(modelMat, glm::vec3(0.1f));
	m_parallaxMapShader->setMatrix("modelMat", modelMat);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void AdvancedLightingState::drawHdr()
{
	m_floatFb->bindFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// �ݲ�����Ч�����⣬�ڴ˶������飬�����Ķ�
	std::vector<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(0.0f, 0.0f, 49.5f)); // back light
	lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
	lightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
	lightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));
	// colors
	std::vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
	lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
	lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_woodTexId);
	// set lighting uniforms
	m_hdrLightShader->use();
	for (unsigned int i = 0; i < lightPositions.size(); i++) {
		m_hdrLightShader->setVec("lights[" + std::to_string(i) + "].position", lightPositions[i]);
		m_hdrLightShader->setVec("lights[" + std::to_string(i) + "].color", lightColors[i]);
	}

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
	model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
	m_hdrLightShader->setMatrix("modelMat", model);
	m_hdrLightShader->setBool("reverseNormals", true);
	drawCube();

	// ���Ƶ�Ĭ�ϻ���
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_floatFb->bindTexture();
	m_hdrShader->use();
	m_hdrShader->setBool("isUseHdr", m_controlParam->isUseHdr);
	m_hdrShader->setFloat("exposure", m_controlParam->exposure);

	drawQuad();
}

void AdvancedLightingState::drawBloom()
{
	// ��ȡ��ɫ
	drawBrightness();

	// ģ������Ƭ�Σ�������˹ģ����
	drawGaussBlurBrightness();

	// ������
	drawMixHdrAndBlur();
}

void AdvancedLightingState::drawBrightness()
{
	m_multAttachFloatFb->bindFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_woodTexId);

	// lighting info �ݲ�����Ч�����⣬�ڴ˶������飬�����Ķ�
	// -------------
	// positions
	std::vector<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
	lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
	lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
	lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
	// colors
	std::vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
	lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
	lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));

	m_bloomShader->use();
	for (int i = 0; i < lightPositions.size(); i++) {
		m_bloomShader->setVec("lights[" + std::to_string(i) + "].position", lightPositions[i]);
		m_bloomShader->setVec("lights[" + std::to_string(i) + "].color", lightColors[i]);
	}
	// create one large cube that acts as the floor
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
	model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
	m_bloomShader->setMatrix("modelMat", model);
	drawCube();

	// then create multiple cubes as the scenery
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	m_bloomShader->setMatrix("modelMat", model);
	drawCube(m_containerTexId);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	m_bloomShader->setMatrix("modelMat", model);
	drawCube(m_containerTexId);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	m_bloomShader->setMatrix("modelMat", model);
	drawCube(m_containerTexId);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
	model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(1.25));
	m_bloomShader->setMatrix("modelMat", model);
	drawCube(m_containerTexId);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
	model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	m_bloomShader->setMatrix("modelMat", model);
	drawCube(m_containerTexId);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	m_bloomShader->setMatrix("modelMat", model);
	drawCube(m_containerTexId);

	// light source
	m_lightBoxShader->use();
	for (int i = 0; i < lightPositions.size(); i++) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPositions[i]);
		model = glm::scale(model, glm::vec3(0.25f));

		m_lightBoxShader->setMatrix("modelMat", model);
		m_lightBoxShader->setVec("lightColor", lightColors[i]);
		drawCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void AdvancedLightingState::drawGaussBlurBrightness()
{
	bool isHorizontal = true;
	bool isFirstRender = true;
	int amount = 10;

	m_blurShader->use();
	for (int i = 0; i < amount; i++) {
		m_pingpongFbs[isHorizontal]->bindFramebuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ע���ٷ��̳��޴��д��룬��Ҫ��ջ������ݣ�����������Ч

		m_blurShader->setBool("isHorizontal", isHorizontal);

		// ��һ����Ⱦ, ping pong����������������, ��һ��������������location = 1��������Ⱦ
		if (isFirstRender) {
			m_multAttachFloatFb->bindTexture(GL_TEXTURE_2D, GL_TEXTURE0, 1);
		}
		else {
			m_pingpongFbs[!isHorizontal]->bindTexture();
		}
		drawQuad();

		isHorizontal = !isHorizontal;

		if (isFirstRender) {
			isFirstRender = false;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void AdvancedLightingState::drawMixHdrAndBlur()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_multAttachFloatFb->bindTexture(GL_TEXTURE_2D, GL_TEXTURE0, 0); // hdr
	m_pingpongFbs[false]->bindTexture(GL_TEXTURE_2D, GL_TEXTURE1, 0); // blur

	m_bloomFinalShader->use();
	m_bloomFinalShader->setBool("isBloom", m_controlParam->isBloom);
	m_bloomFinalShader->setFloat("exposure", m_controlParam->exposure);
	drawQuad();
}

void AdvancedLightingState::drawDeferredShading()
{
	// �ӳ���Ⱦȱ�㣺���ڴ濪����û��MSAA�ͻ��(����Ҫ������Ⱦ�����)

	// geometry pass: render scene into gbuffer
	m_defferedShadingFloatFb->bindFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_geometryPassShader->use();
	for (unsigned int i = 0; i < m_backpackObjectPositions.size(); i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_backpackObjectPositions[i]);
		model = glm::scale(model, glm::vec3(0.25f));
		m_geometryPassShader->setMatrix("modelMat", model);

		m_backpackModel->draw(m_geometryPassShader);
	}

	// lighting pass: get data from gbuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_defferedShadingFloatFb->bindTexture(GL_TEXTURE_2D, GL_TEXTURE0, 0);
	m_defferedShadingFloatFb->bindTexture(GL_TEXTURE_2D, GL_TEXTURE1, 1);
	m_defferedShadingFloatFb->bindTexture(GL_TEXTURE_2D, GL_TEXTURE2, 2);

	m_lightingPassShader->use();
	for (unsigned int i = 0; i < m_lightPositions.size(); i++) {
		std::string lightPrefix = "lights[" + std::to_string(i) + "].";
		m_lightingPassShader->setVec(lightPrefix + "position", m_lightPositions[i]);
		m_lightingPassShader->setVec(lightPrefix + "color", m_lightColors[i]);
		float constant = 1.0f;
		float linear = 0.7f;
		float quadratic = 1.8f;
		m_lightingPassShader->setFloat(lightPrefix + "linear", linear);
		m_lightingPassShader->setFloat(lightPrefix + "quadratic", quadratic);

		// ���ù�ʽ�����������뾶
		// ʵ������GPU��GLSL�����ó��Ż�ѭ���ͷ�֧, һ��if������еķ�֧�Ӷ���֤��ɫ�����ж���һ���ģ���ʹ������֮ǰ�İ뾶����Ż���ȫ������ã�������Ȼ�ڶ����й�Դ�������
		// ����ʹ�ù��������Ҫ��Ⱦһ��ʵ������
		// demo url: https://ogldev.org/
		const float maxBrightness = std::fmaxf(std::fmaxf(m_lightPositions[i].r, m_lightPositions[i].g), m_lightPositions[i].b);
		float radius = (-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2 * quadratic);
		m_lightingPassShader->setFloat(lightPrefix + "radius", radius);
	}
	m_lightingPassShader->setVec("viewPos", m_cameraWrapper->cameraPos());
	drawQuad();

	//  copy content of geometry's depth buffer to default framebuffer's depth buffer
	m_defferedShadingFloatFb->blitFramebuffer(0, GL_DEPTH_BUFFER_BIT);

	// render lights on top of scene;
	m_lightBoxShader->use();
	for (unsigned int i = 0; i < m_lightPositions.size(); i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_lightPositions[i]);
		model = glm::scale(model, glm::vec3(0.125f));
		m_lightBoxShader->setMatrix("modelMat", model);
		m_lightBoxShader->setVec("lightColor", m_lightPositions[i]);
		drawCube();
	}
}

void AdvancedLightingState::drawSsao()
{
	// 1. geometry pass: render scene into gbuffer
	m_ssaoGeometryFloatFb->bindFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// floor cube
	m_ssaoGeometryPassShader->use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
	m_ssaoGeometryPassShader->setMatrix("modelMat", model);
	drawCube();

	// backpack model on the floor
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	m_ssaoGeometryPassShader->setMatrix("modelMat", model);
	m_backpackModel->draw(m_ssaoGeometryPassShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. generate SSAO texture
	// ���㹩������ֱ�Ӽ�����ڱ����ӣ��� 1 - �ڰ����ڱ����ӣ�
	m_ssaoFloatFb->bindFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT);
	// Send kernel + rotation 
	m_ssaoShader->use();
	for (unsigned int i = 0; i < 64; ++i) {
		m_ssaoShader->setVec("samples[" + std::to_string(i) + "]", m_ssaoKernel[i]);
	}
	auto projectionMat = glm::perspective(glm::radians(m_cameraWrapper->fieldOfView()), float(UCDD::kViewportWidth) / UCDD::kViewportHeight, 0.1f, 100.0f);
	m_ssaoShader->setMatrix("projectionMat", projectionMat);

	m_ssaoGeometryFloatFb->bindTexture(GL_TEXTURE_2D, GL_TEXTURE0, 0);
	m_ssaoGeometryFloatFb->bindTexture(GL_TEXTURE_2D, GL_TEXTURE1, 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_ssaoNoiseTexId);
	drawQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. blur SSAO texture to remove noise
	m_ssaoBlurFloatFb->bindFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT);
	m_ssaoBlurShader->use();
	m_ssaoFloatFb->bindTexture(GL_TEXTURE0); // �ڱ����ӻҶ����
	drawQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_ssaoLightingPassShader->use();
	// send light relevant uniforms
	glm::vec3 lightPosView = glm::vec3(m_cameraWrapper->lookAtMatrix() * glm::vec4(m_lightPos, 1.0));
	m_ssaoLightingPassShader->setVec("light.position", lightPosView);
	m_ssaoLightingPassShader->setVec("light.color", m_lightColor);

	// Update attenuation parameters
	const float linear = 0.09f;
	const float quadratic = 0.032f;
	m_ssaoLightingPassShader->setFloat("light.linear", linear);
	m_ssaoLightingPassShader->setFloat("light.quadratic", quadratic);
	m_ssaoGeometryFloatFb->bindTexture(GL_TEXTURE_2D, GL_TEXTURE0, 0);
	m_ssaoGeometryFloatFb->bindTexture(GL_TEXTURE_2D, GL_TEXTURE1, 1);
	m_ssaoGeometryFloatFb->bindTexture(GL_TEXTURE_2D, GL_TEXTURE2, 2);
	m_ssaoBlurFloatFb->bindTexture(GL_TEXTURE_2D, GL_TEXTURE3, 0); // add extra SSAO texture to lighting pass
	drawQuad();
}

