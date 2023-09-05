#include "StateSourceFactory.h"

#include "AdvancedExerciceState.h"
#include "AdvancedLightingState.h"
#include "PbrExerciceState.h"
#include "PracticeExerciceState.h"

StateSourceFactory::StateSourceFactory()
{
	m_stateDictionary[{ GLFW_KEY_B, GLFW_KEY_1 }] = []() { return std::make_unique<SpecialTriangleExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_B, GLFW_KEY_2 }] = []() { return std::make_unique<AdjacentTriangleExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_B, GLFW_KEY_3 }] = []() { return std::make_unique<AdjacentTriangle1ExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_B, GLFW_KEY_4 }] = []() { return std::make_unique<FlipTriangleExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_B, GLFW_KEY_5 }] = []() { return std::make_unique<RichTriangleExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_B, GLFW_KEY_6 }] = []() { return std::make_unique<RectExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_B, GLFW_KEY_7 }] = []() { return std::make_unique<LineRectExerciceState>(); };

	m_stateDictionary[{ GLFW_KEY_C, GLFW_KEY_1 }] = []() { return std::make_unique<TextureExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_C, GLFW_KEY_2 }] = []() { return std::make_unique<TextureMixExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_C, GLFW_KEY_2 }] = []() { return std::make_unique<TextureMixExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_C, GLFW_KEY_3 }] = []() { return std::make_unique<TextureExercice1State>(); };
	m_stateDictionary[{ GLFW_KEY_C, GLFW_KEY_4 }] = []() { return std::make_unique<TextureExercice4State>(); };
	m_stateDictionary[{ GLFW_KEY_C, GLFW_KEY_5 }] = []() { return std::make_unique<TransformTextureExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_C, GLFW_KEY_6 }] = []() { return std::make_unique<Fake3DTextureExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_C, GLFW_KEY_7 }] = []() { return std::make_unique<CubeTextureExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_C, GLFW_KEY_L }] = []() { return std::make_unique<ClearExerciceState>(); };

	m_stateDictionary[{ GLFW_KEY_E, GLFW_KEY_1 }] = []() { return std::make_unique<LightSceneExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_E, GLFW_KEY_2 }] = []() { return std::make_unique<LightMapExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_E, GLFW_KEY_3 }] = []() { return std::make_unique<LightSourceExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_E, GLFW_KEY_4 }] = []() { return std::make_unique<FlashLightSourceExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_E, GLFW_KEY_5 }] = []() { return std::make_unique<MultipleLightSourceExerciceState>(); };

	m_stateDictionary[{ GLFW_KEY_F, GLFW_KEY_1 }] = []() { return std::make_unique<ModelExerciceState>(); };

	m_stateDictionary[{ GLFW_KEY_G, GLFW_KEY_1 }] = []() { return std::make_unique<AdvancedExerciceState>(); };
	
	m_stateDictionary[{ GLFW_KEY_H, GLFW_KEY_1 }] = []() { return std::make_unique<AdvancedLightingState>(); };
	m_stateDictionary[{ GLFW_KEY_I, GLFW_KEY_1 }] = []() { return std::make_unique<PbrExerciceState>(); };
	m_stateDictionary[{ GLFW_KEY_J, GLFW_KEY_1 }] = []() { return std::make_unique<PracticeExerciceState>(); };
}

std::unique_ptr<AbstractExerciceState> StateSourceFactory::stateSource(const std::list<int>& keyList)
{
	std::unique_ptr<AbstractExerciceState> state;

	auto iter = m_stateDictionary.find(keyList);
	if (iter != m_stateDictionary.end()) {
		state = m_stateDictionary[keyList]();
	}

	return state;
}

StateContext::StateContext()
{
	m_stateFactory = std::make_unique<StateSourceFactory>();
}

void StateContext::setPressKeyList(const std::list<int>& keyList)
{
	if (keyList.empty() || m_presskeyList == keyList) {
		return;
	}

	for (int key : keyList) {
		if (key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D) {
			return;
		}
	}

	if (auto stateSource = m_stateFactory->stateSource(keyList)) {
		m_presskeyList = keyList;
		setState(std::move(stateSource));
	}
}

void StateContext::use()
{
	if (!m_state)
		return;

	m_state->draw();
}