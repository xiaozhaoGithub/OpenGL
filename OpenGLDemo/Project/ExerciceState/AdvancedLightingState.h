#ifndef ADVANCEDLIGHTINGSTATE_H
#define ADVANCEDLIGHTINGSTATE_H

#include <memory>

#include "ExerciceState.h"
#include "FramebufferFactory.h"

struct AdvancedLightingStateControlParam;
class AdvancedLightingState : public AbstractExerciceState
{
public:
	AdvancedLightingState();
	~AdvancedLightingState();

	void draw() override;

private:
	void processInput();

	void initDefferedShading();
	void initSsaoShading();

	// common painter
	void drawQuad();

	// business painter
	void drawFloor(std::shared_ptr<AbstractShader> shader);
	void drawCube(unsigned int texId = 0);
	void drawDepthMap();
	void drawSceneToDepthMap(std::shared_ptr<AbstractShader> shader);

	void drawScene(std::shared_ptr<AbstractShader> shader);
	void drawShadow();

	void drawOmnidirectionShadow();
	void drawOmnidirectionShadowScene(std::shared_ptr<AbstractShader> shader);
	void drawNormalMap();
	void drawParallaxMap();
	void drawHdr();

	void drawBloom();
	void drawBrightness();
	void drawGaussBlurBrightness();
	void drawMixHdrAndBlur();

	void drawDeferredShading();
	void drawSsao();

private:
	std::shared_ptr<CameraWrapper> m_cameraWrapper;

	std::unique_ptr<AdvancedLightingStateControlParam> m_controlParam;

	std::shared_ptr<AbstractVAO> m_woodFloorVAO;
	std::shared_ptr<AbstractVAO> m_cubeVAO;
	std::shared_ptr<AbstractVAO> m_quadVAO;
	std::shared_ptr<AbstractVAO> m_normalMapVAO;
	std::shared_ptr<AbstractVAO> m_parallaxMapVAO;

	std::shared_ptr<AbstractShader> m_depthMapShader;
	std::shared_ptr<AbstractShader> m_debugDepthShader;
	std::shared_ptr<AbstractShader> m_shadowShader;
	std::shared_ptr<AbstractShader> m_depthCubeMapShader;
	std::shared_ptr<AbstractShader> m_omnidirectionShadowShader;
	std::shared_ptr<AbstractShader> m_normalMapShader;
	std::shared_ptr<AbstractShader> m_parallaxMapShader;
	std::shared_ptr<AbstractShader> m_hdrLightShader;
	std::shared_ptr<AbstractShader> m_hdrShader;

	std::shared_ptr<AbstractShader> m_bloomShader;
	std::shared_ptr<AbstractShader> m_lightBoxShader;
	std::shared_ptr<AbstractShader> m_blurShader;
	std::shared_ptr<AbstractShader> m_bloomFinalShader;

	std::shared_ptr<Framebuffer> m_depthMapFb;
	std::shared_ptr<Framebuffer> m_cubeMapDepthFb;
	std::shared_ptr<Framebuffer> m_floatFb;
	std::shared_ptr<Framebuffer> m_multAttachFloatFb;
	std::vector<std::shared_ptr<Framebuffer>> m_pingpongFbs;

	unsigned int m_woodTexId;
	unsigned int m_containerTexId;

	std::shared_ptr<AbstractShader> m_geometryPassShader;
	std::shared_ptr<AbstractShader> m_lightingPassShader;
	std::unique_ptr<Model> m_backpackModel;
	std::vector<glm::vec3> m_backpackObjectPositions;
	std::vector<glm::vec3> m_lightPositions;
	std::vector<glm::vec3> m_lightColors;
	std::shared_ptr<Framebuffer> m_defferedShadingFloatFb;

	std::shared_ptr<AbstractShader> m_ssaoGeometryPassShader;
	std::shared_ptr<AbstractShader> m_ssaoShader;
	std::shared_ptr<AbstractShader> m_ssaoBlurShader;
	std::shared_ptr<AbstractShader> m_ssaoLightingPassShader;
	std::shared_ptr<Framebuffer> m_ssaoGeometryFloatFb;
	std::shared_ptr<Framebuffer> m_ssaoFloatFb;
	std::shared_ptr<Framebuffer> m_ssaoBlurFloatFb;
	std::vector<glm::vec3> m_ssaoKernel;
	unsigned int m_ssaoNoiseTexId;
	glm::vec3 m_lightPos;
	glm::vec3 m_lightColor;
};	

#endif