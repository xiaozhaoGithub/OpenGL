#ifndef RENDERLOOP_H_
#define RENDERLOOP_H_

#include <string>
#include <list>
#include <functional>

#include "glad/glad.h"
// ��ȷ�����ڰ���GLFW��ͷ�ļ�֮ǰ������GLAD��ͷ�ļ���GLAD��ͷ�ļ���������ȷ��OpenGLͷ�ļ�������GL/gl.h����
// ������Ҫ������������OpenGL��ͷ�ļ�֮ǰ����GLAD��
#include "glfw3.h"
#include "Shader.h"
#include "DataModel/VAOFactory.h"
#include "ShaderExerciceState.h"

class RenderLoop
{
public:
	RenderLoop() {}
	int exec();

private:
	int initFramework();
	void initShader();
	void initVAO();
	void initDrawFun();
	void initAttribute();
	void startLoop();

	void processInput(GLFWwindow *window);
	const std::list<std::function<void()>> drawFuntionList(int model, int subModel = 0);
	void draw(std::function<void()> drawHandle);
	void draw(std::list<std::function<void()>> drawHandleList);


private:
	std::shared_ptr<AbstractShader> m_shaderProgram;
	std::shared_ptr<AbstractShader> m_flipShaderProgram;
	std::shared_ptr<AbstractShader>  m_yellowShaderProgram;
	std::shared_ptr<AbstractShader>  m_uniformTestShaderProgram;
	std::shared_ptr<AbstractShader>  m_withColorShaderProgram;

	//std::unique_ptr<StateContext>  m_stateContext;
	// VAO Manager
	unsigned int m_normalTriangleVAO;
	unsigned int m_rectVAO;

	unsigned int m_specialTriangleVAO;
	unsigned int m_adjacentTriangleVAO;
	unsigned int m_adjacentTriangle1VAO;
	unsigned int m_adjacentTriangle2VAO;

	unsigned int m_richTriangleVAO;
	std::shared_ptr<AbstractVAO> m_textureTriangleVAO;
	std::shared_ptr<AbstractVAO> m_textureUnitTriangleVAO;
	std::shared_ptr<AbstractVAO> m_textureTransformTriangleVAO;
	std::shared_ptr<AbstractVAO> m_textureFake3DTriangleVAO;
	std::shared_ptr<AbstractVAO> m_texture3DTriangleVAO;

	int m_model;
	int m_subModel;
	std::map<int, std::map<int, std::list<std::function<void()> > > > m_drawFunMap;
};

#endif

