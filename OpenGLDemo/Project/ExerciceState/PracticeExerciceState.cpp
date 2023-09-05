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

	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	//auto viewMat = m_cameraWrapper->lookAtMatrix();

	//// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
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

	// 宽度值设为0表示我们要从字体面通过给定的高度中动态计算出字形的宽度
	FT_Set_Pixel_Sizes(face, 0, 48);

	// FT_LOAD_RENDER, 告诉FreeType去创建一个8位的灰度位图
	if (FT_Load_Char(face, 'x', FT_LOAD_RENDER) != 0)
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

	// OpenGL要求所有的纹理都是4字节对齐的，即纹理的大小永远是4字节的倍数。通常这并不会出现什么问题，因为大部分纹理的宽度都为4的倍数或
	// 每像素使用4个字节，但是现在我们每个像素只用了一个字节，它可以是任意的宽度。通过将纹理解压对齐参数设为1，这样才能确保不会有对齐问题
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制

	for (GLubyte c = 0; c < 128; c++) {
		// 加载字符的字形 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0) {
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		// 生成纹理
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
		// 设置纹理选项
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 储存字符供之后使用
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

	// 注: 需要使用浮点数的参数构建投影矩阵，否则文字绘制不生效
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
	// 激活对应的渲染状态
	shader->use();
	shader->setVec("textColor", color);

	glActiveTexture(GL_TEXTURE0);

	m_textVao->bindVAO();

	// 遍历文本中所有的字符
	for (auto c = text.begin(); c != text.end(); c++) {
		Character ch = m_characters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale; // 基准线以下的部分需要考虑

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;
		// 对每个字符更新VBO
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// 在四边形上绘制字形纹理
		glBindTexture(GL_TEXTURE_2D, ch.textureId);

		// update content of VBO memory
		m_textVao->fillBufSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// 更新位置到下一个字形的原点，注意单位是1/64像素
		x += (ch.advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
