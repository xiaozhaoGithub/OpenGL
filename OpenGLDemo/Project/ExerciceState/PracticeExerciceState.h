#ifndef PRACTICEEXERCICESTATE_H_
#define PRACTICEEXERCICESTATE_H_

#include <memory>

#include "ExerciceState.h"
#include "FramebufferFactory.h"

struct Character {
	GLuint     textureId;  // ���������ID
	glm::ivec2 size;       // ���δ�С
	glm::ivec2 bearing;    // �ӻ�׼�ߵ�������/������ƫ��ֵ
	GLuint     advance;    // ԭ�����һ������ԭ��ľ���
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