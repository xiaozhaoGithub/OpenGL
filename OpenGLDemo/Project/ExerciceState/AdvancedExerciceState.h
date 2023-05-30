#ifndef ADVANCEDEXERCICESTATE_H_
#define ADVANCEDEXERCICESTATE_H_

#include <memory>

#include "ExerciceState.h"
#include "FramebufferFactory.h"

/**
 * @brief ����һ���в�͸����͸������ĳ�����ʱ�� �����ԭ������
 *
 * 1.�Ȼ������в�͸�������塣
 * 2.������͸������������
 * 3.��˳���������͸�������塣
 */
class AdvancedExerciceState : public AbstractExerciceState
{
public:
	AdvancedExerciceState();
	~AdvancedExerciceState();

	void draw() override;

private:
	// painter
	void drawDepthTest();
	void drawFloor();
	void drawCube();
	void drawVegetation();
	void drawWindow();
	void drawFramebuffer();
	void drawPostProcessCube();
	void drawReflectedCube();
	void drawModel();
	void drawReflectMapModel();
	void drawSkybox();
	void drawPoints();
	void drawFragCoordCube();
	void drawGeometryLines();
	void drawGeometryTriangles();
	void drawExplodeModel();
	void drawNormalVisibleModel();

	// helper
	void setSampler(std::shared_ptr<AbstractShader> shader);
	void initUniformBlock();

private:
	std::shared_ptr<AbstractVAO> m_cubeVAO;
	std::shared_ptr<AbstractVAO> m_planeVAO;
	std::shared_ptr<AbstractVAO> m_vegetationVAO;
	std::shared_ptr<AbstractVAO> m_windowVAO;
	std::shared_ptr<AbstractVAO> m_quadVAO;
	std::shared_ptr<AbstractVAO> m_postProcessCubeVAO;
	std::shared_ptr<AbstractVAO> m_skyboxVAO;
	std::shared_ptr<AbstractVAO> m_reflectedCubeVAO;
	std::unique_ptr<Model> m_nanosuitModel;
	std::shared_ptr<AbstractVAO> m_pointVAO;
	std::shared_ptr<AbstractVAO> m_pointsVAO;

	std::shared_ptr<Framebuffer> m_sceneFramebuffer;

	std::shared_ptr<AbstractShader> m_depthTestShader; // ��Ȳ��Կ��ӻ�
	std::shared_ptr<AbstractShader> m_singleColorShader;
	std::shared_ptr<AbstractShader> m_transparentShader;
	std::shared_ptr<AbstractShader> m_screenShader;
	std::shared_ptr<AbstractShader> m_cubeMapShader;
	std::shared_ptr<AbstractShader> m_reflectedCubeShader;
	std::shared_ptr<AbstractShader> m_refractShader;
	std::shared_ptr<AbstractShader> m_reflectMapShader;
	std::shared_ptr<AbstractShader> m_pointsShader;
	std::shared_ptr<AbstractShader> m_fragCoordShader;
	std::shared_ptr<AbstractShader> m_linesShader;
	std::shared_ptr<AbstractShader> m_houseShader;
	std::shared_ptr<AbstractShader> m_explodeShader;
	std::shared_ptr<AbstractShader> m_normalVisibleShader;

	unsigned int m_skyboxTexId;
	unsigned int m_uboExampleBlock;

	std::vector<glm::vec3> m_vegetationPos;
	std::vector<glm::vec3> m_windowPos;

	glm::mat4 m_viewMat;
	glm::mat4 m_projectionMat;
};

#endif