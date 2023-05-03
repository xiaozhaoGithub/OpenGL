#ifndef CAMERAWRAPPER_H_
#define CAMERAWRAPPER_H_

#include "glad/glad.h"
#include "glfw3.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "Singleton.h"

class CameraWrapper
{
	SINGLETON_DECLARE(CameraWrapper)
public:
	enum Mode {
		M_FREE,
		M_GROUND
	};

	CameraWrapper();
	CameraWrapper(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
	~CameraWrapper() {}

	void setWindow(GLFWwindow* window) { m_window = window; }
	void setMode(Mode mode) { m_mode = mode; }
	void setCursorVisible(bool isVisible);

	void processKeyPress();
	void processScroll(float xoffset, float yoffset);
	void processCursor(float xPos, float yPos);

	// lookAt第二个参数：摄像机前方观察目标的位置（始终不变）
	glm::mat4 lookAtMatrix() { return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFrontPos, m_upPos); }
	glm::mat4 customLookAtMatrix() { return customLookAt(m_cameraPos, m_cameraPos + m_cameraFrontPos, m_upPos); }

	float fieldOfView() { return m_fieldOfView; }
	glm::vec3 cameraPos() { return m_cameraPos; }
	glm::vec3 cameraFrontPos() { return m_cameraFrontPos; }

private:
	glm::mat4 customLookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);

private:
	GLFWwindow* m_window;
	Mode m_mode;
	bool m_cursorIsVisible;

	glm::mat4 m_lookAtMat; // 摄像机为原点的坐标系与平移方向向量的组合，lookAt与任何向量相乘都会将该向量转化到摄像机坐标系
	glm::vec3 m_cameraPos;
	glm::vec3 m_cameraFrontPos;
	glm::vec3 m_upPos;

	float m_fieldOfView;
	float m_lastFrame;

	bool m_firstMouse;
	float m_lastX;
	float m_lastY;

	float m_pitch;
	float m_yaw;

	// 目前我们的移动速度是个常量。理论上没问题，但实际情况下根据处理器的能力不同，有些人可能会比其他人每秒绘制更多帧，以更高的频率调用processInput函数。
	// 结果就是，根据配置的不同，有些人可能移动很快，而有些人会移动很慢。当你发布你的程序的时候，你必须确保它在所有硬件上移动速度都一样。
	// 图形程序和游戏通常会跟踪一个时间差(Deltatime)变量，它储存了渲染上一帧所用的时间。我们把所有速度都去乘以deltaTime值。结果就是，如果我们
	// 的deltaTime很大，就意味着上一帧的渲染花费了更多时间，所以这一帧的速度需要变得更高来平衡渲染所花去的时间。使用这种方法时，无论你的电脑快还是慢，
	// 摄像机的速度都会相应平衡，这样每个用户的体验就都一样了。
	float m_cameraSpeed;
};

inline CameraWrapper::CameraWrapper()
	: CameraWrapper(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f))
{
}

inline CameraWrapper::CameraWrapper(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
	: m_firstMouse(true)
	, m_mode(M_FREE)
	, m_cursorIsVisible(false)
{
	m_cameraPos = eye;
	m_cameraFrontPos = center;
	m_upPos = up;

	m_cameraSpeed = 2.5f;

	m_fieldOfView = 45.0f;
	
	m_pitch = 0.0f;
	m_yaw = -90.0f;// 初始的偏航角为-90度, 目的是确定摄像机方向向量（0，0，-1）
}

inline void CameraWrapper::processScroll(float xoffset, float yoffset)
{
	(void)xoffset;
	int rate = 2;

	if (m_fieldOfView >= 0.0f && m_fieldOfView <= 45.0f) {
		m_fieldOfView -= (yoffset * rate);
	}

	if (m_fieldOfView < 1.0f) {
		m_fieldOfView = 1.0f;
	}
	if (m_fieldOfView > 45.0f) {
		m_fieldOfView = 45.0f;
	}
}

inline void CameraWrapper::processCursor(float xpos, float ypos)
{
	if (m_firstMouse) {
		m_lastX = xpos;
		m_lastY = ypos;
		m_firstMouse = false;
	}

	// 1 计算光标偏移量
	float xoffset = xpos - m_lastX;
	float yoffset = m_lastY - ypos; // reversed since y-coordinates go from bottom to top
	m_lastX = xpos;
	m_lastY = ypos;

	float sensitivity = 0.05f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	// 2 偏移量应用到俯仰角、偏航角
	m_pitch += yoffset;
	m_yaw += xoffset;

	// 3 俯仰角边界值处理
	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (m_pitch > 89.0f)
		m_pitch = 89.0f;
	if (m_pitch < -89.0f)
		m_pitch = -89.0f;

	// 4 通过俯仰角和偏航角来计算以得到真正的摄像机方向向量
	glm::vec3 camerafront;
	camerafront.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
	camerafront.y = sin(glm::radians(m_pitch));
	camerafront.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));

	m_cameraFrontPos = glm::normalize(camerafront); // 单位、方向向量（向量各分量除以模值，最终模值为1）
}

inline glm::mat4 CameraWrapper::customLookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
	glm::vec3 zVec = glm::normalize(eye - center);
	auto xVec = glm::normalize(glm::cross(glm::normalize(up), zVec));
	auto yVec = glm::normalize(glm::cross(zVec, xVec));
	 yVec = glm::cross(zVec, xVec);

	glm::mat4 translationMat = glm::mat4(1.0f); // col 为主要
	translationMat[3][0] = -eye.x;
	translationMat[3][1] = -eye.y;
	translationMat[3][2] = -eye.z;

	glm::mat4 rotateMat = glm::mat4(1.0f); 
	rotateMat[0][0] = xVec.x;
	rotateMat[1][0] = xVec.y;
	rotateMat[2][0] = xVec.z;
	rotateMat[0][1] = yVec.x;
	rotateMat[1][1] = yVec.y;
	rotateMat[2][1] = yVec.z;
	rotateMat[0][2] = zVec.x;
	rotateMat[1][2] = zVec.y;
	rotateMat[2][2] = zVec.z;

	return rotateMat * translationMat;
}

inline void CameraWrapper::setCursorVisible(bool isVisible)
{
	m_cursorIsVisible = isVisible;
	glfwSetInputMode(m_window, GLFW_CURSOR, isVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

inline void CameraWrapper::processKeyPress()
{
	float currentFrame = (float)glfwGetTime();
	float deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	float realSpeed = m_cameraSpeed * deltaTime;

	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
		m_cameraPos += realSpeed * m_cameraFrontPos;
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
		m_cameraPos -= realSpeed * m_cameraFrontPos;
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
		m_cameraPos -= glm::cross(m_cameraFrontPos, m_upPos) * realSpeed;
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
		m_cameraPos += glm::cross(m_cameraFrontPos, m_upPos) * realSpeed;
	}

	switch (m_mode) {
	case M_FREE:
		break;
	case M_GROUND:
		m_cameraPos.y = 0;
		break;
	default:
		break;
	}
}

#endif

