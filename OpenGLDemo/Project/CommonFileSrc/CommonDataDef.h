#ifndef COMMONDATADEF_H_
#define COMMONDATADEF_H_
#include <iostream>
#include "glm.hpp"

namespace UiCommonDataDef
{
	extern const int kViewportWidth;
	extern const int kViewportHeight;
	extern const int kShadowWidth;
	extern const int kShadowHeight;
}

namespace ExerciceStateCommonDataDef 
{
	extern const glm::vec3 cubePositions[];
	extern const glm::vec3 pointLightPositions[];
	extern const glm::vec3 pointLightColors[];
}

#endif