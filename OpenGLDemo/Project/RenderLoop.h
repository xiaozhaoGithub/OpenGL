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
#include "ExerciceState.h"
#include "StateSourceFactory.h"

class RenderLoop
{
public:
	RenderLoop();
	int exec();

private:
	int initFramework();
	void startLoop();

	void processInput(GLFWwindow *window);

private:
	std::unique_ptr<StateContext> m_stateContext;
};

#endif

