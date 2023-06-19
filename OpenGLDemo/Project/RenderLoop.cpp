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
	//glEnable(GL_MULTISAMPLE); // �������ز����������OpenGL�������ϣ����ز�������Ĭ�����õģ�
	Singleton<CameraWrapper>::instance()->setWindow(g_globalWindow);

	startLoop();

	return 0;
}

int RenderLoop::initFramework()
{
	// GLFW���ڴ���OpenGL 3.3 �����ģ�������Ϊ����ģʽ
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4); // ÿ������4���Ӳ�����, �����
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

	// GLAD��������ȡOpenGL�ĺ���ָ�루��ʹ��GLAD��Ҫ�ֶ���ȡOpengGL������ַ������ֵ���Ҳ�ͬϵͳ��ȡ��ʽ��һ�£�
	// GLFW�����ǵ���glfwGetProcAddress��������ϵͳ��ص�OpenGL����ָ���ַ�ĺ���
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// OpenGL��Ⱦ���ڵĳߴ��С�����ӿ�(Viewport)
	// glViewport����ǰ������������viewport��Դ������½ǵ�λ��
	glViewport(0, 0, UiCommonDataDef::kViewportWidth, UiCommonDataDef::kViewportHeight);

		
	// ע�ᴰ�ڴ�С�仯�ص�
	glfwSetFramebufferSizeCallback(g_globalWindow, frameBufferSizeCallback);
	glfwSetCursorPosCallback(g_globalWindow, onCursorPosChanged);
	glfwSetScrollCallback(g_globalWindow, onScrollChanged);

	return 0;
}

void RenderLoop::startLoop()
{
	while (!glfwWindowShouldClose(g_globalWindow)) {
		// ��Ⱦָ��
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// ��������
		processInput(g_globalWindow);
		 
		// ��鲢�����¼����������壨ǰ/�󽻻���
		glfwSwapBuffers(g_globalWindow);
		glfwPollEvents();
	}

	// TODO ����������Դ�ͷ�����
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
