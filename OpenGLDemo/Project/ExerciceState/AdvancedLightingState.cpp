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
	bool isShowShadow = false;
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

	m_depthMapFb = FramebufferFactory::createDepthFb();
	m_cubeMapDepthFb = FramebufferFactory::createCubeMapDepthFb();
	
	m_woodTexId = TextureHelper::loadTexture("skin/textures/wood.png");

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

	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	auto viewMat = m_cameraWrapper->lookAtMatrix();

	// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
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

	if (glfwGetKey(g_globalWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
		m_controlParam->isShowShadow = !m_controlParam->isShowShadow;
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

	// 1.渲染深度贴图，是从光的透视图里渲染的深度纹理，用它计算阴影
	// 阴影贴图经常和我们原来渲染的场景（通常是窗口分辨率）有着不同的分辨率，我们需要改变视口（viewport）的参数以适应阴影贴图的尺寸。
	// 如果我们忘了更新视口参数，最后的深度贴图要么太小要么就不完整。
	glViewport(0, 0, UCDD::kShadowWidth, UCDD::kShadowHeight);
	m_depthMapFb->bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT); // 深度帧缓冲清理深度缓冲

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

	// 2.使用深度贴图渲染场景
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

	// 1.渲染深度贴图，是从光的透视图里渲染的深度纹理，用它计算阴影
	// 阴影贴图经常和我们原来渲染的场景（通常是窗口分辨率）有着不同的分辨率，我们需要改变视口（viewport）的参数以适应阴影贴图的尺寸。
	// 如果我们忘了更新视口参数，最后的深度贴图要么太小要么就不完整。
	glViewport(0, 0, UCDD::kShadowWidth, UCDD::kShadowHeight);
	m_depthMapFb->bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT); // 深度帧缓冲清理深度缓冲

	glm::mat4 modelMat = glm::mat4(1.0f);
	shader->setMatrix("modelMat", modelMat);

	m_woodFloorVAO->bindVAO();
	m_woodFloorVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// 使用正面剔除，可以使深度贴图的深度值（即最近的深度值）变大，大到边缘点，确保阴影更准确
	// 接近阴影的物体仍然会出现不正确的效果。必须考虑到何时使用正面剔除对物体才有意义。不过使用普通的偏移值通常就能避免peter panning。
	// 因使用合适的计算偏移方式，此处先不使用正面剔除（阴影与物体交汇处仍存在问题）
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

	// 1.渲染深度贴图，是从光的透视图里渲染的深度纹理，用它计算阴影
	// 阴影贴图经常和我们原来渲染的场景（通常是窗口分辨率）有着不同的分辨率，我们需要改变视口（viewport）的参数以适应阴影贴图的尺寸。
	// 如果我们忘了更新视口参数，最后的深度贴图要么太小要么就不完整。
	glViewport(0, 0, UCDD::kShadowWidth, UCDD::kShadowHeight);
	m_depthMapFb->bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT); // 深度帧缓冲清理深度缓冲

	drawSceneToDepthMap(m_depthMapShader);

	// reset viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, UCDD::kViewportWidth, UCDD::kViewportHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2.再次渲染场景
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

	// 90度我们才能保证视野足够大到可以合适地填满立方体贴图的一个面
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


