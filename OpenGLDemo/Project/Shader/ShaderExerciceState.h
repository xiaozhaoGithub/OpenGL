#ifndef SHADEREXERCICESTATE_H_
#define SHADEREXERCICESTATE_H_

#include <memory>
#include <list>

#include "glad/glad.h"
#include "glfw3.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "Shader.h"
#include "ShaderFactory.h"
#include "VAOFactory.h"
#include "CameraWrapper.h"
#include "Model.h"

class StateContext;
class AbstractExerciceState
{
public:
	AbstractExerciceState() { ; }
	virtual ~AbstractExerciceState() {}

	virtual void use(StateContext*) {}; // do action

	void setFloat(const std::string& name, float value) { m_shader->setFloat(name, value); }
	void setMatrix(const std::string& name, float* value) { m_shader->setMatrix(name, value); }

protected:
	bool match(const std::list<int>& keyList1st, const std::list<int>& keyList2nd);

protected:
	std::shared_ptr<AbstractShader> m_shader;
};

// state pattern
class StateContext
{
public:
	StateContext();
	virtual ~StateContext() {}

	void use();

	void setState(std::unique_ptr<class AbstractExerciceState> state) { m_state = std::move(state); }
	void setFloat(const std::string& name, float value) { m_state->setFloat(name, value); }
	void setMatrix(const std::string& name, float* value) { m_state->setMatrix(name, value); }

	void setPressKey(int key);
	int pressKey() { return m_presskey; }

	void setPressKeyList(const std::list<int>& keyList);
	const std::list<int>& pressKeyList() { return m_presskeyList; }

	void setVao(std::shared_ptr<AbstractVAO> vao) { m_vao = vao; }
	std::shared_ptr<AbstractVAO> vao() { return m_vao; }

private:
	std::unique_ptr<AbstractExerciceState> m_state;

	int m_presskey = 0;
	std::list<int> m_presskeyList;
	std::shared_ptr<AbstractVAO> m_vao;
};

class AbstractTextureExerciceState : public AbstractExerciceState
{
public:
	AbstractTextureExerciceState() : AbstractExerciceState() {}
	virtual ~AbstractTextureExerciceState() {}
};

class AbstractLightExerciceState : public AbstractExerciceState
{
public:
	AbstractLightExerciceState() : AbstractExerciceState() {}
	virtual ~AbstractLightExerciceState() {}
};

class AbstractModelExerciceState : public AbstractExerciceState
{
public:
	AbstractModelExerciceState() : AbstractExerciceState() {}
	virtual ~AbstractModelExerciceState() {}
};


class TextureExerciceStateOther : public AbstractTextureExerciceState
{
public:
	TextureExerciceStateOther() = default;

	void use(StateContext* context) override;
};

class TextureExerciceState : public AbstractTextureExerciceState
{
public:
	TextureExerciceState();

	void use(StateContext* context) override;
};

class TextureExerciceStateMix : public AbstractTextureExerciceState
{
public:
	TextureExerciceStateMix();

	void use(StateContext* context) override;
};

class TextureExerciceState1 : public AbstractTextureExerciceState
{
public:
	TextureExerciceState1();
	void use(StateContext* context) override;
};

class TextureExerciceState4 : public AbstractTextureExerciceState
{
public:
	TextureExerciceState4();
	void use(StateContext* context) override;
};

class TransformTextureExerciceState : public AbstractTextureExerciceState
{
public:
	TransformTextureExerciceState();
	void use(StateContext* context) override;
};

class TextureExerciceState3D : public AbstractTextureExerciceState
{
public:
	TextureExerciceState3D();
	void use(StateContext* context) override;
};

class TextureExerciceStateCube : public AbstractTextureExerciceState
{
public:
	TextureExerciceStateCube();
	void use(StateContext* context) override;
}; 

class LightSceneExerciceState : public AbstractLightExerciceState
{
public:
	LightSceneExerciceState();
	void use(StateContext* context) override;

private:
	std::shared_ptr<AbstractShader> m_lightShader;
	std::shared_ptr<AbstractShader> m_targetShader;

	std::shared_ptr<AbstractVAO> m_lightVAO;
	std::shared_ptr<AbstractVAO> m_targetVAO;

	glm::vec3 m_lightPos;
	glm::vec4 m_lightColor;
	glm::vec4 m_targetColor;
};

class LightMapExerciceState : public AbstractLightExerciceState
{
public:
	LightMapExerciceState();
	void use(StateContext* context) override;

private:
	std::shared_ptr<AbstractShader> m_lightShader;
	std::shared_ptr<AbstractShader> m_targetShader;

	std::shared_ptr<AbstractVAO> m_lightVAO;
	std::shared_ptr<AbstractVAO> m_targetVAO;

	glm::vec3 m_lightPos;
	glm::vec4 m_lightColor;
	//glm::vec4 m_targetColor; 纹理记载图片，无目标颜色
};

class LightSourceExerciceState : public AbstractLightExerciceState
{
public:
	LightSourceExerciceState();
	void use(StateContext* context) override;

private:
	std::shared_ptr<AbstractShader> m_lightShader;
	std::shared_ptr<AbstractShader> m_targetShader;

	std::shared_ptr<AbstractVAO> m_lightVAO;
	std::shared_ptr<AbstractVAO> m_targetVAO;

	glm::vec3 m_lightPos;
	glm::vec4 m_lightColor;
	//glm::vec4 m_targetColor; 纹理记载图片，无目标颜色
};

class FlashLightSourceExerciceState : public AbstractLightExerciceState
{
public:
	FlashLightSourceExerciceState();
	void use(StateContext* context) override;

private:
	std::shared_ptr<AbstractShader> m_lightShader;
	std::shared_ptr<AbstractShader> m_targetShader;

	std::shared_ptr<AbstractVAO> m_lightVAO;
	std::shared_ptr<AbstractVAO> m_targetVAO;

	glm::vec3 m_lightPos;
	glm::vec4 m_lightColor;
	//glm::vec4 m_targetColor; 纹理记载图片，无目标颜色
};

class MultipleLightSourceExerciceState : public AbstractLightExerciceState
{
public:
	MultipleLightSourceExerciceState();
	~MultipleLightSourceExerciceState();
	void use(StateContext* context) override;

private:
	std::shared_ptr<AbstractShader> m_lightShader;
	std::shared_ptr<AbstractShader> m_targetShader;

	std::shared_ptr<AbstractVAO> m_lightVAO;
	std::shared_ptr<AbstractVAO> m_targetVAO;

	glm::vec3 m_lightPos;
	glm::vec4 m_lightColor;

	glm::vec3 *pointLightPositions;
	glm::vec3 *pointLightColors;
};

class ModelExerciceState : public AbstractModelExerciceState
{
public:
	ModelExerciceState();
	void use(StateContext* context) override;

private:
	std::unique_ptr<Model> m_model;

	std::shared_ptr<AbstractShader> m_lightShader;
	std::shared_ptr<AbstractShader> m_targetShader;
	std::shared_ptr<AbstractShader> m_modelShader;

	std::shared_ptr<AbstractVAO> m_lightVAO;
	std::shared_ptr<AbstractVAO> m_targetVAO;

	glm::vec3 m_lightPos;
	glm::vec4 m_lightColor;
};

#endif

