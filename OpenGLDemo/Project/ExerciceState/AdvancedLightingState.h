#ifndef ADVANCEDLIGHTINGSTATE_H
#define ADVANCEDLIGHTINGSTATE_H

#include <memory>

#include "ExerciceState.h"
#include "FramebufferFactory.h"

class AdvancedLightingState : public AbstractExerciceState
{
public:
	AdvancedLightingState();
	AdvancedLightingState(bool isBlinn);
	~AdvancedLightingState();

	void draw() override;

private:
	// painter
	void drawFloor();

	std::shared_ptr<AbstractVAO> m_woodFloorVAO;
	bool m_isBlinn;
};

#endif