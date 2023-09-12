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
	void initIblSpecular();

	// common painter
	//void drawQuad();
	void drawSphere();
	void drawCube();
	void drawSkybox();
	void drawQuad();

	// business painter
	void drawPbrLighting(std::shared_ptr<AbstractShader> shader);
	void drawPbrLightingTexture();

	void drawIblIrradianceConversion();
	void drawEquiRectMapToCubemap();
	void drawIblIrradianceMap();
	void drawPreFilterMap();
	void draw2DLutTexture();

	void drawPbrLightingConvolution();
	void drawIblSpecular();

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

	glm::mat4 m_captureProjection;
	std::vector<glm::mat4> m_captureViews;
	std::shared_ptr<AbstractShader> m_equiRectToCubemapShader;
	std::shared_ptr<AbstractShader> m_irradianceConvolutionShader;
	std::shared_ptr<AbstractShader> m_backgroundShader;
	std::shared_ptr<AbstractShader> m_pbrLightConvolutionShader;
	std::shared_ptr<Framebuffer> m_captureFb;
	unsigned int m_equiRectMapTexId;
	unsigned int m_envCubemapTexId;
	unsigned int m_irradianceMapTexId;
	std::shared_ptr<AbstractVAO> m_cubeVAO;

	std::shared_ptr<AbstractVAO> m_quadVAO;
	std::shared_ptr<AbstractShader> m_iblLightShader;
	std::shared_ptr<AbstractShader> m_prefilterShader;
	std::shared_ptr<AbstractShader> m_brdfShader;
	unsigned int m_prefilterMapTexId;
	unsigned int m_brdfLutTexId;
};	

#endif