#include "PracticeExerciceState.h"

#include "Singleton.h"
#include "CommonDataDef.h"
#include "TextureHelper.h"
#include "ft2build.h"
#include FT_FREETYPE_H

extern GLFWwindow* g_globalWindow;

namespace UCDD = UiCommonDataDef;

PracticeExerciceState::PracticeExerciceState()
	: AbstractExerciceState()
{
	m_cameraWrapper = Singleton<CameraWrapper>::instance();
	m_shaderFactory = Singleton<ShaderFactory>::instance();

	//initTransformMatUniformBlock();
	initFont();
	initText();
}

void PracticeExerciceState::render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_cameraWrapper->setCursorVisible(false);

	// ģ�;�����Ҫ����ƽ�ơ����š���ת��һ������������ģ�;��󣬵õ�����ռ��еĶ���λ��

	// �۲������Ҫ������任���۲���������ռ�
	//auto viewMat = m_cameraWrapper->lookAtMatrix();

	//// ͶӰ����, �ü��ɼ����꣬������͸��Ч����Զ�����������η���Խ�󣬶���ԽС
	//auto projectionMat = glm::perspective(glm::radians(m_cameraWrapper->fieldOfView()), float(UCDD::kViewportWidth) / UCDD::kViewportHeight, 0.1f, 100.0f);

	//glBindBuffer(GL_UNIFORM_BUFFER, m_transformMatUbo);
	//glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMat));
	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectionMat));

	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	drawTitle(m_textShader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
	drawTitle(m_textShader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
}

void PracticeExerciceState::initFont()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft) != 0)
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "skin/font/arial.ttf", 0, &face) != 0)
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// ���ֵ��Ϊ0��ʾ����Ҫ��������ͨ�������ĸ߶��ж�̬��������εĿ��
	FT_Set_Pixel_Sizes(face, 0, 48);

	// FT_LOAD_RENDER, ����FreeTypeȥ����һ��8λ�ĻҶ�λͼ
	if (FT_Load_Char(face, 'x', FT_LOAD_RENDER) != 0)
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

	// OpenGLҪ�����е�������4�ֽڶ���ģ�������Ĵ�С��Զ��4�ֽڵı�����ͨ���Ⲣ�������ʲô���⣬��Ϊ�󲿷�����Ŀ�ȶ�Ϊ4�ı�����
	// ÿ����ʹ��4���ֽڣ�������������ÿ������ֻ����һ���ֽڣ�������������Ŀ�ȡ�ͨ���������ѹ���������Ϊ1����������ȷ�������ж�������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //�����ֽڶ�������

	for (GLubyte c = 0; c < 128; c++) {
		// �����ַ������� 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		// ��������
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// ��������ѡ��
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// �����ַ���֮��ʹ��
		Character character = {
			textureId,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};
		m_characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void PracticeExerciceState::initText()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_textVao = Singleton<TriangleVAOFactory>::instance()->createTextVAO();

	// ע: ��Ҫʹ�ø������Ĳ�������ͶӰ���󣬷������ֻ��Ʋ���Ч
	auto projectionMat = glm::ortho(0.0f, (float)UCDD::kViewportWidth, 0.0f, (float)UCDD::kViewportHeight);

	m_textShader = m_shaderFactory->shaderProgram("practice_text", "ShaderProgram/practice/practice_text.vs", "ShaderProgram/practice/practice_text.fs");
	m_textShader->use();
	m_textShader->setMatrix("projectionMat", projectionMat);
}

void PracticeExerciceState::drawTitle(std::shared_ptr<AbstractShader> shader,
	const std::string& text, 
	GLfloat x, 
	GLfloat y,
	GLfloat scale, 
	glm::vec3 color)
{
	// �����Ӧ����Ⱦ״̬
	shader->use();
	shader->setVec("textColor", color);

	glActiveTexture(GL_TEXTURE0);

	m_textVao->bindVAO();

	// �����ı������е��ַ�
	for (auto c = text.begin(); c != text.end(); c++) {
		Character ch = m_characters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale; // ��׼�����µĲ�����Ҫ����

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;
		// ��ÿ���ַ�����VBO
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// ���ı����ϻ�����������
		glBindTexture(GL_TEXTURE_2D, ch.textureId);

		// update content of VBO memory
		m_textVao->fillBufSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// ����λ�õ���һ�����ε�ԭ�㣬ע�ⵥλ��1/64����
		x += (ch.advance >> 6) * scale; // λƫ��6����λ����ȡ��λΪ���ص�ֵ (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
