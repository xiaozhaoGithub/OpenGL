#include "RenderLoop.h"

#include <iostream>
#include <memory>

#include "Shader.h"
#include "ShaderFactory.h"
#include "VAOFactory.h"
#include "CommonDataDef.h"

GLFWwindow* g_globalWindow;

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void onCursorPosChanged(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	Singleton<CameraWrapper>::instance()->processCursor(static_cast<float>(xposIn), static_cast<float>(yposIn));
}

void onScrollChanged(GLFWwindow* window, double xoffset, double yoffset)
{
	Singleton<CameraWrapper>::instance()->processScroll(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

RenderLoop::RenderLoop()
{
	m_stateContext = std::make_unique<StateContext>();
}

int RenderLoop::exec()
{
	bool result = initFramework();
	if (result != 0)
		return result;

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_MULTISAMPLE); // 开启多重采样（大多数OpenGL的驱动上，多重采样都是默认启用的）
	Singleton<CameraWrapper>::instance()->setWindow(g_globalWindow);

	startLoop();

	return 0;
}

int RenderLoop::initFramework()
{
	// GLFW会在创建OpenGL 3.3 上下文，并设置为核心模式
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4); // 每个像素4个子采样点, 抗锯齿
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#ifdef __MAC__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	g_globalWindow = glfwCreateWindow(UiCommonDataDef::kViewportWidth, UiCommonDataDef::kViewportHeight, "LearnOpenGL", NULL, NULL);

	if (!g_globalWindow) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (auto monitor = glfwGetPrimaryMonitor()) {
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		int x = (mode->width - UiCommonDataDef::kViewportWidth) >> 1;
		int y = (mode->height - UiCommonDataDef::kViewportHeight) >> 1;
		glfwSetWindowPos(g_globalWindow, x, y);
		glfwShowWindow(g_globalWindow);
	}

	glfwMakeContextCurrent(g_globalWindow);

	// GLAD是用来获取OpenGL的函数指针（不使用GLAD需要手动获取OpengGL函数地址，并赋值，且不同系统获取方式不一致）
	// GLFW给我们的是glfwGetProcAddress用来加载系统相关的OpenGL函数指针地址的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// OpenGL渲染窗口的尺寸大小，即视口(Viewport)
	// glViewport函数前两个参数控制viewport相对窗口左下角的位置
	glViewport(0, 0, UiCommonDataDef::kViewportWidth, UiCommonDataDef::kViewportHeight);

		
	// 注册窗口大小变化回调
	glfwSetFramebufferSizeCallback(g_globalWindow, frameBufferSizeCallback);
	glfwSetCursorPosCallback(g_globalWindow, onCursorPosChanged);
	glfwSetScrollCallback(g_globalWindow, onScrollChanged);

	return 0;
}

void RenderLoop::startLoop()
{
	while (!glfwWindowShouldClose(g_globalWindow)) {
		// 渲染指令
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// 按键输入
		processInput(g_globalWindow);
		 
		// 检查并调用事件、交换缓冲（前/后交换）
		glfwSwapBuffers(g_globalWindow);
		glfwPollEvents();
	}

	// TODO 类来管理资源释放问题
	//glDeleteVertexArrays(1, &m_normalTriangleVAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteProgram(m_shaderProgram);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
}

void RenderLoop::processInput(GLFWwindow *window)
{
	Singleton<CameraWrapper>::instance()->processKeyPress();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	auto fillPressKey2sd = [window](int key, std::list<int>& keyList) {
		if (glfwGetKey(window, key) == GLFW_PRESS) {
			keyList.push_back(key);
		}
	};

	auto fillPressKey1st = [fillPressKey2sd, window](int key, std::list<int>& keyList) {
		if (glfwGetKey(window, key) == GLFW_PRESS) {
			keyList.push_back(key);

			for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; key++) {
				fillPressKey2sd(key, keyList);
			}
		}
	};

	std::list<int> keyList;
	for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; key++) {
		fillPressKey1st(key, keyList);
	}

	m_stateContext->setPressKeyList(keyList);
	m_stateContext->use();
}
