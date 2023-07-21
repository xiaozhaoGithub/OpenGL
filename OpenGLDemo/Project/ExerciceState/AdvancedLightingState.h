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

	// painter
	void drawFloor(std::shared_ptr<AbstractShader> shader);
	void drawCube();
	void drawDepthMap();
	void drawSceneToDepthMap(std::shared_ptr<AbstractShader> shader);

	void drawScene(std::shared_ptr<AbstractShader> shader);
	void drawShadow();

	void drawOmnidirectionShadow();
	void drawOmnidirectionShadowScene(std::shared_ptr<AbstractShader> shader);
	void drawNormalMap();

private:
	std::shared_ptr<CameraWrapper> m_cameraWrapper;

	std::unique_ptr<AdvancedLightingStateControlParam> m_controlParam;

	std::shared_ptr<AbstractVAO> m_woodFloorVAO;
	std::shared_ptr<AbstractVAO> m_cubeVAO;
	std::shared_ptr<AbstractVAO> m_quadVAO;
	std::shared_ptr<AbstractVAO> m_normalMapVAO;

	std::shared_ptr<AbstractShader> m_depthMapShader;
	std::shared_ptr<AbstractShader> m_debugDepthShader;
	std::shared_ptr<AbstractShader> m_shadowShader;
	std::shared_ptr<AbstractShader> m_depthCubeMapShader;
	std::shared_ptr<AbstractShader> m_omnidirectionShadowShader;
	std::shared_ptr<AbstractShader> m_normalMapShader;

	std::shared_ptr<Framebuffer> m_depthMapFb;
	std::shared_ptr<Framebuffer> m_cubeMapDepthFb;

	unsigned int m_woodTexId;
};

#endif