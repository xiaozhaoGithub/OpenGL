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

	// lookAt�ڶ��������������ǰ���۲�Ŀ���λ�ã�ʼ�ղ��䣩
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

	glm::mat4 m_lookAtMat; // �����Ϊԭ�������ϵ��ƽ�Ʒ�����������ϣ�lookAt���κ�������˶��Ὣ������ת�������������ϵ
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

	// Ŀǰ���ǵ��ƶ��ٶ��Ǹ�������������û���⣬��ʵ������¸��ݴ�������������ͬ����Щ�˿��ܻ��������ÿ����Ƹ���֡���Ը��ߵ�Ƶ�ʵ���processInput������
	// ������ǣ��������õĲ�ͬ����Щ�˿����ƶ��ܿ죬����Щ�˻��ƶ����������㷢����ĳ����ʱ�������ȷ����������Ӳ�����ƶ��ٶȶ�һ����
	// ͼ�γ������Ϸͨ�������һ��ʱ���(Deltatime)����������������Ⱦ��һ֡���õ�ʱ�䡣���ǰ������ٶȶ�ȥ����deltaTimeֵ��������ǣ��������
	// ��deltaTime�ܴ󣬾���ζ����һ֡����Ⱦ�����˸���ʱ�䣬������һ֡���ٶ���Ҫ��ø�����ƽ����Ⱦ����ȥ��ʱ�䡣ʹ�����ַ���ʱ��������ĵ��Կ컹������
	// ��������ٶȶ�����Ӧƽ�⣬����ÿ���û�������Ͷ�һ���ˡ�
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
	m_yaw = -90.0f;// ��ʼ��ƫ����Ϊ-90��, Ŀ����ȷ�����������������0��0��-1��
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

	// 1 ������ƫ����
	float xoffset = xpos - m_lastX;
	float yoffset = m_lastY - ypos; // reversed since y-coordinates go from bottom to top
	m_lastX = xpos;
	m_lastY = ypos;

	float sensitivity = 0.05f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	// 2 ƫ����Ӧ�õ������ǡ�ƫ����
	m_pitch += yoffset;
	m_yaw += xoffset;

	// 3 �����Ǳ߽�ֵ����
	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (m_pitch > 89.0f)
		m_pitch = 89.0f;
	if (m_pitch < -89.0f)
		m_pitch = -89.0f;

	// 4 ͨ�������Ǻ�ƫ�����������Եõ��������������������
	glm::vec3 camerafront;
	camerafront.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
	camerafront.y = sin(glm::radians(m_pitch));
	camerafront.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));

	m_cameraFrontPos = glm::normalize(camerafront); // ��λ��������������������������ģֵ������ģֵΪ1��
}

inline glm::mat4 CameraWrapper::customLookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
	glm::vec3 zVec = glm::normalize(eye - center);
	auto xVec = glm::normalize(glm::cross(glm::normalize(up), zVec));
	auto yVec = glm::normalize(glm::cross(zVec, xVec));
	 yVec = glm::cross(zVec, xVec);

	glm::mat4 translationMat = glm::mat4(1.0f); // col Ϊ��Ҫ
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

