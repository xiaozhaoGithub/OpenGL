#include "RenderLoop.h"

#include <iostream>
#include <memory>

#include "Shader.h"
#include "ShaderFactory.h"
#include "VAOFactory.h"

GLFWwindow* m_globalWindow;
std::unique_ptr<StateContext> m_stateContext;

CameraWrapper m_cameraWrapper(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void onCursorPos(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	m_cameraWrapper.processCursor(static_cast<float>(xposIn), static_cast<float>(yposIn));
}

void onScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	m_cameraWrapper.processScroll(static_cast<float>(xoffset), static_cast<float>(yoffset));
}

int RenderLoop::exec()
{
	bool result = initFramework();
	if (result != 0)
		return result;

	initShader();
	initVAO();
	initDrawFun();
	initAttribute();

	m_cameraWrapper.setWindow(m_globalWindow);
	//m_cameraWrapper.setMode(CameraWrapper::M_GROUND);

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
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	m_globalWindow = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (!m_globalWindow) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(m_globalWindow);

	// GLAD是用来获取OpenGL的函数指针（不使用GLAD需要手动获取OpengGL函数地址，并赋值，且不同系统获取方式不一致）
	// GLFW给我们的是glfwGetProcAddress用来加载系统相关的OpenGL函数指针地址的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// OpenGL渲染窗口的尺寸大小，即视口(Viewport)
	// glViewport函数前两个参数控制窗口左下角的位置
	glViewport(0, 0, 800, 600);

	// 注册窗口大小变化回调
	glfwSetFramebufferSizeCallback(m_globalWindow, frameBufferSizeCallback);
	glfwSetCursorPosCallback(m_globalWindow, onCursorPos);
	glfwSetScrollCallback(m_globalWindow, onScroll);

	return 0;
}

void RenderLoop::initShader()
{
	// 渲染准备工作、渲染操作
	std::shared_ptr<ShaderFactory> shaderFactory = std::make_shared<ShaderFactory>();
	m_shaderProgram = shaderFactory->shaderProgram("normal", "ShaderProgram/shader.vs", "ShaderProgram/shader.fs");
	m_yellowShaderProgram = shaderFactory->shaderProgram("yellow", "ShaderProgram/shader.vs", "ShaderProgram/yellowShader.fs");
	m_uniformTestShaderProgram = shaderFactory->shaderProgram("uniform", "ShaderProgram/shader.vs", "ShaderProgram/uniformTestShader.fs");
	m_withColorShaderProgram = shaderFactory->shaderProgram("withColor", "ShaderProgram/vertex_with_color_shader.vs", "ShaderProgram/vertex_with_color_shader.fs");
	m_flipShaderProgram = shaderFactory->shaderProgram("flip", "ShaderProgram/flip_shader.vs", "ShaderProgram/shader.fs");
	
	m_stateContext = std::make_unique<StateContext>();
}

void RenderLoop::initVAO()
{
	// 生成多种VAO
	auto triangleVAOFactory = std::make_unique<TriangleVAOFactory>();
	m_normalTriangleVAO = triangleVAOFactory->createNormalVAO();

	m_specialTriangleVAO = triangleVAOFactory->createSpecialVAO();
	m_adjacentTriangleVAO = triangleVAOFactory->createAdjacentVAO();
	m_adjacentTriangle1VAO = triangleVAOFactory->createAdjacent2VAO().first;
	m_adjacentTriangle2VAO = triangleVAOFactory->createAdjacent2VAO().second;
	m_richTriangleVAO = triangleVAOFactory->createRichVAO();
	m_textureTriangleVAO = triangleVAOFactory->creatTextureVAO();
	m_textureUnitTriangleVAO = triangleVAOFactory->creatMixTextureUnitVAO();
	m_textureTransformTriangleVAO = triangleVAOFactory->createTransformVAO();
	m_textureFake3DTriangleVAO = triangleVAOFactory->createTransformVAO();
	m_texture3DTriangleVAO = triangleVAOFactory->create3DVAO();

	auto rectVAOFactory = std::make_shared<RectVAOFactory>();
	m_rectVAO = rectVAOFactory->createNormalVAO();

	int maxVertexAttribs = 0;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs); // 16
}

void RenderLoop::initDrawFun()
{
	m_drawFunMap[GLFW_KEY_TAB][GLFW_KEY_0] = { [this]() {
		glBindVertexArray(m_specialTriangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	} };

	m_drawFunMap[GLFW_KEY_SPACE][GLFW_KEY_0] = { [this]() {
	glBindVertexArray(m_rectVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); }
	};

	m_drawFunMap[GLFW_KEY_Q][GLFW_KEY_0] = { [this]() {
		glBindVertexArray(m_adjacentTriangle1VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		m_yellowShaderProgram->use();

		glBindVertexArray(m_adjacentTriangle2VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3); }
	};

	m_drawFunMap[GLFW_KEY_B][GLFW_KEY_0] = { [this]() {
		// 可以看到，uniform对于设置一个在渲染迭代中会改变的属性是一个非常有用的工具，它也是一个在程序和着色器间数据交互的很好工具，
		// 但假如我们打算为每个顶点设置一个颜色的时候该怎么办？
		// 这种情况下，我们就不得不声明和顶点数目一样多的uniform了。在这一问题上更好的解决方案是在顶点属性中包含更多的数据
		float timeValue = (float)glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

		m_uniformTestShaderProgram->use();
		m_uniformTestShaderProgram->setFloat("outColor", 0.0f, greenValue, 0.0f, 1.0f);

		glBindVertexArray(m_normalTriangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	} };

	m_drawFunMap[GLFW_KEY_C][GLFW_KEY_0] = { [this]() {
		glBindVertexArray(m_rectVAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	} };

	m_drawFunMap[GLFW_KEY_P][GLFW_KEY_0] = { [this]() {
		glBindVertexArray(m_adjacentTriangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	} };

	m_drawFunMap[GLFW_KEY_E][GLFW_KEY_0] = { [this]() {
		m_withColorShaderProgram->use();
		m_withColorShaderProgram->setFloat("xOffset", 0.5f);
		glBindVertexArray(m_richTriangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	} };

	m_drawFunMap[GLFW_KEY_F][GLFW_KEY_0] = { [this]() {
		m_flipShaderProgram->use();
		glBindVertexArray(m_normalTriangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	} };

	m_drawFunMap[GLFW_KEY_G][GLFW_KEY_0] = { [this]() {
		m_stateContext->setPressKey(GLFW_KEY_0);
		m_stateContext->use();
		m_textureTriangleVAO->bindVAO();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	} };

	
	auto textTureDrawFun = [this]() {
		m_stateContext->use();
		m_textureUnitTriangleVAO->bindTexture();
		m_textureUnitTriangleVAO->bindVAO();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	};

	m_drawFunMap[GLFW_KEY_H][GLFW_KEY_0] = { [this]() {
		m_stateContext->setPressKey(GLFW_KEY_N);

		m_stateContext->use();

		m_textureUnitTriangleVAO->bindTexture();
		m_textureUnitTriangleVAO->bindVAO();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	} };
	m_drawFunMap[GLFW_KEY_H][GLFW_KEY_1] = { [this]() {
		m_stateContext->setPressKey(GLFW_KEY_1);
	}, textTureDrawFun };
	m_drawFunMap[GLFW_KEY_H][GLFW_KEY_2] = { [this]() {
		m_stateContext->setPressKey(GLFW_KEY_2);
	}, textTureDrawFun };

	m_drawFunMap[GLFW_KEY_H][GLFW_KEY_4] = { [this]() {
		m_stateContext->setPressKey(GLFW_KEY_4);

		m_stateContext->use();
		static float opacity = 0.2f;
		if (glfwGetKey(m_globalWindow, GLFW_KEY_UP) == GLFW_PRESS) {
			opacity += 0.01f;
			if (opacity >= 1.0f) {
				opacity = 1.0f;
			}
			m_stateContext->setFloat("opacity", opacity);
		}
		if (glfwGetKey(m_globalWindow, GLFW_KEY_P) == GLFW_PRESS) {
			opacity -= 0.01f;
			if (opacity <= 0.0f) {
				opacity = 0.0f;
			}
			m_stateContext->setFloat("opacity", opacity);
		}
	}, textTureDrawFun };

	m_drawFunMap[GLFW_KEY_H][GLFW_KEY_5] = { [this]() {
		m_stateContext->setPressKey(GLFW_KEY_5);
		m_stateContext->setVao(m_textureTransformTriangleVAO);
		m_stateContext->use();
	} };

	m_drawFunMap[GLFW_KEY_H][GLFW_KEY_6] = { [this]() {
		m_stateContext->setPressKey(GLFW_KEY_6);
		m_stateContext->setVao(m_textureFake3DTriangleVAO);
		m_stateContext->use();
	} };

	m_drawFunMap[GLFW_KEY_H][GLFW_KEY_7] = { [this]() {
		m_stateContext->setPressKey(GLFW_KEY_7);
		m_stateContext->setVao(m_texture3DTriangleVAO);
		m_stateContext->use();
	} };
}

void RenderLoop::initAttribute()
{
	glEnable(GL_DEPTH_TEST);
}

void RenderLoop::startLoop()
{
	// 渲染循环(Render Loop)
	while (!glfwWindowShouldClose(m_globalWindow)) {
		// 渲染指令
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// 渲染时需要着色器程序
		//m_shaderProgram->use();
		//glBindVertexArray(m_normalTriangleVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		// 按键输入
		processInput(m_globalWindow);

		//draw(drawFuntionList(m_model, m_subModel));
		 
		// 检查并调用事件、交换缓冲（前/后交换）
		glfwSwapBuffers(m_globalWindow);
		glfwPollEvents();
	}

	// 渲染循环结束后我们需要正确释放/删除之前的分配的所有资源
	glDeleteVertexArrays(1, &m_normalTriangleVAO);
	glDeleteVertexArrays(1, &m_specialTriangleVAO);
	glDeleteVertexArrays(1, &m_rectVAO);

	// 类来管理释放
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &nextVBO);
	//glDeleteBuffers(1, &rectVBO);

	//glDeleteProgram(m_shaderProgram);
	//glDeleteProgram(m_yellowShaderProgram);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
}

void RenderLoop::draw(std::function<void()> drawHandle)
{
	std::list<std::function<void()>> list;
	list.emplace_back(drawHandle);

	draw(list);
}

void RenderLoop::draw(std::list<std::function<void()>> drawHandleList)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (const auto fun : drawHandleList) {
		fun();
	}
}

void RenderLoop::processInput(GLFWwindow *window)
{
	m_cameraWrapper.processKeyPress();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		m_model = GLFW_KEY_TAB;
		m_subModel = GLFW_KEY_0;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		m_model = GLFW_KEY_SPACE;
		m_subModel = GLFW_KEY_0;
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		m_model = GLFW_KEY_Q;
		m_subModel = GLFW_KEY_0;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		m_model = GLFW_KEY_B;
		m_subModel = GLFW_KEY_0;
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		m_model = GLFW_KEY_C;
		m_subModel = GLFW_KEY_0;
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		m_model = GLFW_KEY_P;
		m_subModel = GLFW_KEY_0;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		m_model = GLFW_KEY_E;
		m_subModel = GLFW_KEY_0;
	}
	
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		m_model = GLFW_KEY_F;
		m_subModel = GLFW_KEY_0;
	}	
	
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		m_model = GLFW_KEY_G;
		m_subModel = GLFW_KEY_0;
	}	
	
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		m_model = GLFW_KEY_H;
	}

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
		m_subModel = GLFW_KEY_0;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		m_subModel = GLFW_KEY_1;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		m_subModel = GLFW_KEY_2;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		m_subModel = GLFW_KEY_4;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
		m_subModel = GLFW_KEY_5;
	}	
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
		m_subModel = GLFW_KEY_6;
	}
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
		m_subModel = GLFW_KEY_7;
	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		m_stateContext->setPressKey(GLFW_KEY_L);

		std::list<int> list;
		list.push_back(GLFW_KEY_L);

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			list.push_back(GLFW_KEY_1);
		}
		else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			list.push_back(GLFW_KEY_2);
		}
		else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
			list.push_back(GLFW_KEY_3);
		}
		else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
			list.push_back(GLFW_KEY_4);
		}

		m_stateContext->setPressKeyList(list);
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		m_stateContext->setPressKey(GLFW_KEY_M);

		std::list<int> list;
		list.push_back(GLFW_KEY_M);
		m_stateContext->setPressKeyList(list);
	}

	m_stateContext->use();
}

const std::list<std::function<void()>> RenderLoop::drawFuntionList(int model, int subModel)
{
	auto iterModel = m_drawFunMap.find(model);
	if (iterModel != m_drawFunMap.end()) {
		 auto iterSubModel = iterModel->second.find(subModel);
		 if (iterSubModel != iterModel->second.end()) {
			 return iterSubModel->second;
		 }
	}

	auto errorFun = [model, subModel]() {
		//std::cout << "Didn't find function from the draw fun map. model: " << model << "sub model: " << subModel << std::endl;
	};
	
	std::list<std::function<void()>> errorReturn;
	errorReturn.push_back(errorFun);

	return errorReturn;
}
