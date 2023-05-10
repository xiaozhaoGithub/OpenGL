#ifndef ADVANCEDEXERCICESTATE_H_
#define ADVANCEDEXERCICESTATE_H_

#include <memory>

#include "ExerciceState.h"

class AdvancedExerciceState : public AbstractExerciceState
{
public:
	AdvancedExerciceState();
	void draw() override;

private:
	void drawVegetation();

private:
	std::shared_ptr<AbstractVAO> m_cubeVAO;
	std::shared_ptr<AbstractVAO> m_planeVAO;
	std::shared_ptr<AbstractVAO> m_vegetationVAO;

	std::shared_ptr<AbstractShader> m_singleColorShader;

	std::vector<glm::vec3> m_vegetationPos;
};

#endif