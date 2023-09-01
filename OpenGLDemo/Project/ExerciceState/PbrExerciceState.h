#ifndef PBREXERCICESTATE_H_
#define PBREXERCICESTATE_H_

#include <memory>

#include "ExerciceState.h"
#include "FramebufferFactory.h"

class PbrExerciceState : public AbstractExerciceState
{
public:
	PbrExerciceState();
	~PbrExerciceState(){}

	void render() override;

private:
	void initLighting();
	void initLightingTexture();

	// common painter
	//void drawQuad();
	void drawSphere();

	// business painter
	void drawPbrLighting();
	void drawPbrLightingTexture();

private:
	std::shared_ptr<CameraWrapper> m_cameraWrapper;
	std::shared_ptr<ShaderFactory> m_shaderFactory;

	std::shared_ptr<AbstractVAO> m_sphereVAO;
	std::shared_ptr<AbstractShader> m_pbrLightShader;
	std::vector<glm::vec3> m_lightPositions;
	std::vector<glm::vec3> m_lightColors;

	std::shared_ptr<AbstractShader> m_pbrLightTexShader;
	unsigned int m_sphereAlbedoTexId;
	unsigned int m_sphereNormalTexId;
	unsigned int m_sphereMetallicTexId;
	unsigned int m_sphereRoughnessTexId;
	unsigned int m_sphereAoTexId;
};	

#endif