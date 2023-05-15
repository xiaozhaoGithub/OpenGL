#ifndef ADVANCEDEXERCICESTATE_H_
#define ADVANCEDEXERCICESTATE_H_

#include <memory>

#include "ExerciceState.h"
#include "FramebufferFactory.h"

/**
 * @brief 绘制一个有不透明和透明物体的场景的时候， 大体的原则如下
 *
 * 1.先绘制所有不透明的物体。
 * 2.对所有透明的物体排序。
 * 3.按顺序绘制所有透明的物体。
 */
class AdvancedExerciceState : public AbstractExerciceState
{
public:
	AdvancedExerciceState();
	void draw() override;

private:
	void drawDepthTest();
	void drawFloor();
	void drawCube();
	void drawVegetation();
	void drawWindow();
	void drawFramebuffer();
	void drawPostProcessCube();
	void drawSkybox();

private:
	std::shared_ptr<AbstractVAO> m_cubeVAO;
	std::shared_ptr<AbstractVAO> m_planeVAO;
	std::shared_ptr<AbstractVAO> m_vegetationVAO;
	std::shared_ptr<AbstractVAO> m_windowVAO;
	std::shared_ptr<AbstractVAO> m_quadVAO;
	std::shared_ptr<AbstractVAO> m_postProcessCubeVAO;
	std::shared_ptr<AbstractVAO> m_skyboxVAO;

	std::shared_ptr<Framebuffer> m_sceneFramebuffer;

	std::shared_ptr<AbstractShader> m_depthTestShader; // 深度测试可视化
	std::shared_ptr<AbstractShader> m_singleColorShader;
	std::shared_ptr<AbstractShader> m_transparentShader;
	std::shared_ptr<AbstractShader> m_screenShader;
	std::shared_ptr<AbstractShader> m_cubeMapShader;

	std::vector<glm::vec3> m_vegetationPos;
	std::vector<glm::vec3> m_windowPos;

	glm::mat4 m_viewMat;
	glm::mat4 m_projectionMat;
};

#endif