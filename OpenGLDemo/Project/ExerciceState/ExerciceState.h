#ifndef EXERCICESTATE_H_
#define EXERCICESTATE_H_

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

/**
 * @brief 练习状态，即不同阶段的demo绘制
 */
class AbstractExerciceState
{
public:
	AbstractExerciceState() {}
	virtual ~AbstractExerciceState() {}

	virtual void draw() = 0; // do action

	void setFloat(const std::string& name, float value) { m_shader->setFloat(name, value); }
	void setMatrix(const std::string& name, float* value) { m_shader->setMatrix(name, value); }

protected:
	// TODO 考虑是否只调用一次
	void initTransformMatUniformBlock();

	std::shared_ptr<AbstractShader> m_shader;
	unsigned int m_transformMatUbo;
};

class AbstractTriangleExerciceState : public AbstractExerciceState
{
public:
	AbstractTriangleExerciceState() : AbstractExerciceState() {}
	virtual ~AbstractTriangleExerciceState() {}
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

class ClearExerciceState : public AbstractExerciceState
{
public:
	ClearExerciceState();
	void draw() override;

private:
	unsigned int m_specialTriangleVAO;
};

class SpecialTriangleExerciceState : public AbstractTriangleExerciceState
{
public:
	SpecialTriangleExerciceState();
	void draw() override;

private:
	unsigned int m_specialTriangleVAO;
};

class AdjacentTriangleExerciceState : public AbstractTriangleExerciceState
{
public:
	AdjacentTriangleExerciceState();
	void draw() override;

private:
	unsigned int m_adjacentTriangleVAO;
};

class AdjacentTriangle1ExerciceState : public AbstractTriangleExerciceState
{
public:
	AdjacentTriangle1ExerciceState();
	void draw() override;

private:
	unsigned int m_adjacentTriangle1VAO;
	unsigned int m_adjacentTriangle2VAO;

	std::shared_ptr<AbstractShader>  m_yellowShaderProgram;
};

class FlipTriangleExerciceState : public AbstractTriangleExerciceState
{
public:
	FlipTriangleExerciceState();
	void draw() override;

private:
	std::shared_ptr<AbstractVAO> m_normalTriangleVAO;
};

class RichTriangleExerciceState : public AbstractTriangleExerciceState
{
public:
	RichTriangleExerciceState();
	void draw() override;

private:
	unsigned int m_richTriangleVAO;
	std::shared_ptr<AbstractShader>  m_withColorShaderProgram;
};

class RectExerciceState : public AbstractExerciceState
{
public:
	RectExerciceState();
	void draw() override;

protected:
	std::shared_ptr<AbstractVAO> m_rectVAO;
};

class LineRectExerciceState : public RectExerciceState
{
public:
	LineRectExerciceState();
	void draw() override;
};

class TextureExerciceState : public AbstractTextureExerciceState
{
public:
	TextureExerciceState();
	void draw() override;

private:
	std::shared_ptr<AbstractVAO> m_textureTriangleVAO;
};

class TextureMixExerciceState : public AbstractTextureExerciceState
{
public:
	TextureMixExerciceState();
	void draw() override;

private:
	std::shared_ptr<AbstractVAO> m_textureUnitTriangleVAO;
};

class TextureExercice1State : public AbstractTextureExerciceState
{
public:
	TextureExercice1State();
	void draw() override;

private:
	std::shared_ptr<AbstractVAO> m_textureUnitTriangleVAO;
};

class TextureExercice4State : public AbstractTextureExerciceState
{
public:
	TextureExercice4State();
	void draw() override;

private:
	std::shared_ptr<AbstractVAO> m_textureUnitTriangleVAO;
	float m_opacity;
};

class TransformTextureExerciceState : public AbstractTextureExerciceState
{
public:
	TransformTextureExerciceState();
	void draw() override;

private:
	std::shared_ptr<AbstractVAO> m_textureTransformTriangleVAO;
};

class Fake3DTextureExerciceState : public AbstractTextureExerciceState
{
public:
	Fake3DTextureExerciceState();
	void draw() override;

private:
	std::shared_ptr<AbstractVAO> m_textureFake3DTriangleVAO;
};

class CubeTextureExerciceState : public AbstractTextureExerciceState
{
public:
	CubeTextureExerciceState();	
	void draw() override;
private:
	std::shared_ptr<AbstractVAO> m_texture3DTriangleVAO;
}; 

class LightSceneExerciceState : public AbstractLightExerciceState
{
public:
	LightSceneExerciceState();
	void draw() override;

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
	void draw() override;

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
	void draw() override;

private:
	std::shared_ptr<AbstractShader> m_lightShader;
	std::shared_ptr<AbstractShader> m_targetShader;

	std::shared_ptr<AbstractVAO> m_lightVAO;
	std::shared_ptr<AbstractVAO> m_targetVAO;

	glm::vec3 m_lightPos;
	glm::vec4 m_lightColor;
};

class FlashLightSourceExerciceState : public AbstractLightExerciceState
{
public:
	FlashLightSourceExerciceState();
	void draw() override;

private:
	std::shared_ptr<AbstractShader> m_lightShader;
	std::shared_ptr<AbstractShader> m_targetShader;

	std::shared_ptr<AbstractVAO> m_lightVAO;
	std::shared_ptr<AbstractVAO> m_targetVAO;

	glm::vec3 m_lightPos;
	glm::vec4 m_lightColor;
};

class MultipleLightSourceExerciceState : public AbstractLightExerciceState
{
public:
	MultipleLightSourceExerciceState();
	void draw() override;

private:
	std::shared_ptr<AbstractShader> m_lightShader;
	std::shared_ptr<AbstractShader> m_targetShader;

	std::shared_ptr<AbstractVAO> m_lightVAO;
	std::shared_ptr<AbstractVAO> m_targetVAO;

	glm::vec3 m_lightPos;
	glm::vec4 m_lightColor;
};

class ModelExerciceState : public AbstractModelExerciceState
{
public:
	ModelExerciceState();
	void draw() override;

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

