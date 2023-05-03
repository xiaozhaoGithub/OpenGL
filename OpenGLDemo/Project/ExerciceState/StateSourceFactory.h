#ifndef STATESOURCEFACTORY_H_
#define STATESOURCEFACTORY_H_

#include <memory>
#include <functional>
#include <map>

#include "ExerciceState.h"

/**
 * @brief 状态工厂，根据组合按键返回状态
 */
class StateSourceFactory
{
public:
	StateSourceFactory();
	std::unique_ptr<AbstractExerciceState> stateSource(const std::list<int>& stateKeyList);

private:
	std::map<std::list<int>, std::function<std::unique_ptr<AbstractExerciceState>()> > m_stateDictionary;
};

/**
 * @brief 状态上下文，对外接口，根据键盘输入组合切换状态
 */
class StateContext
{
public:
	StateContext();
	virtual ~StateContext() {}

	void use();

	void setState(std::unique_ptr<AbstractExerciceState> state) { m_state = std::move(state); }

	void setPressKeyList(const std::list<int>& keyList);
	const std::list<int>& pressKeyList() { return m_presskeyList; }

private:
	std::unique_ptr<StateSourceFactory> m_stateFactory;
	std::unique_ptr<AbstractExerciceState> m_state;
	std::list<int> m_presskeyList;
};

#endif