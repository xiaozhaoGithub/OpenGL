#ifndef PRACTICEEXERCICESTATE_H_
#define PRACTICEEXERCICESTATE_H_

#include <memory>

#include "ExerciceState.h"
#include "FramebufferFactory.h"

struct Character {
	GLuint     textureId;  // 字形纹理的ID
	glm::ivec2 size;       // 字形大小
	glm::ivec2 bearing;    // 从基准线到字形左部/顶部的偏移值
	GLuint     advance;    // 原点距下一个字形原点的距离
};

class PracticeExerciceState : public AbstractExerciceState
{
public:
	PracticeExerciceState();
	~PracticeExerciceState(){}

	void render() override;

private:
	void initFont();
	void initText();

	// common painter

	// business painter
	void drawTitle(std::shared_ptr<AbstractShader> shader, 
		const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

private:
	std::shared_ptr<CameraWrapper> m_cameraWrapper;
	std::shared_ptr<ShaderFactory> m_shaderFactory;

	std::map<char, Character> m_characters;
	std::shared_ptr<AbstractShader> m_textShader;
	std::shared_ptr<AbstractVAO> m_textVao;
};	

#endif