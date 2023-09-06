#ifndef PBREXERCICESTATE_H_
#define PBREXERCICESTATE_H_

#include <memory>

#include "ExerciceState.h"
#include "FramebufferFactory.h"

class PbrExerciceState : public AbstractExerciceState
{
public:
	PbrExerciceState();
	~PbrExerciceState();

	void render() override;

private:
	void initLighting();
	void initLightingTexture();
	void initIblIrradianceConversion();

	// common painter
	//void drawQuad();
	void drawSphere();
	void drawCube();

	// business painter
	void drawPbrLighting();
	void drawPbrLightingTexture();

	void drawIblIrradianceConversion();
	void drawEquiRectMapToCubemap();

private:
	std::shared_ptr<CameraWrapper> m_cameraWrapper;
	std::shared_ptr<ShaderFactory> m_shaderFactory;

	int m_rows;
	int m_cols;
	float m_spacing;

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

	std::shared_ptr<AbstractShader> m_equiRectToCubemapShader;
	std::shared_ptr<AbstractShader> m_backgroundShader;
	std::shared_ptr<Framebuffer> m_captureFb;
	unsigned int m_equiRectMapTexId;
	unsigned int m_envCubemapTexId;
	std::shared_ptr<AbstractVAO> m_cubeVAO;
};	

#endif