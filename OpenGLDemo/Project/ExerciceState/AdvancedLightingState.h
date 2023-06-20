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
	void drawFloor();

	std::shared_ptr<AbstractVAO> m_woodFloorVAO;

	std::unique_ptr<AdvancedLightingStateControlParam> m_controlParam;
};

#endif