#ifndef STATESOURCEFACTORY_H_
#define STATESOURCEFACTORY_H_

#include <memory>
#include <functional>
#include <map>

#include "ExerciceState.h"

/**
 * @brief ״̬������������ϰ�������״̬
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
 * @brief ״̬�����ģ�����ӿڣ����ݼ�����������л�״̬
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