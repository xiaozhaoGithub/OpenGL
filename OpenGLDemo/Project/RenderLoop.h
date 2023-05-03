#ifndef RENDERLOOP_H_
#define RENDERLOOP_H_

#include <string>
#include <list>
#include <functional>

#include "glad/glad.h"
// 请确认是在包含GLFW的头文件之前包含了GLAD的头文件。GLAD的头文件包含了正确的OpenGL头文件（例如GL/gl.h），
// 所以需要在其它依赖于OpenGL的头文件之前包含GLAD。
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

