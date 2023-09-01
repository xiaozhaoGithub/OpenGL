#include "Shader.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "glad/glad.h"

AbstractShader::AbstractShader(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
{
	// compile vertex shader
	std::fstream vertexStream(vertexPath, std::ios_base::in);
	if (!vertexStream.is_open()) {
		std::cout << "Open vertex shader file fail. path: " << vertexPath << std::endl;
	}

	std::stringstream strVertexStream;
	strVertexStream << vertexStream.rdbuf();
	vertexStream.close();

	std::string tmpSouce = strVertexStream.str(); // 必须先转成std::string类型, 再c_str, 否则乱码
	const char* vertexSource = tmpSouce.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	int success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "vertex shader compile fail. reason: " << infoLog << std::endl;
	}

	// compile geometry shader (optional)
	unsigned int geometryShader;
	if (geometryPath) {
		std::fstream geometryStream(geometryPath, std::ios_base::in);
		if (!geometryStream.is_open()) {
			std::cout << "Open geometry shader file fail. path: " << fragmentPath << std::endl;
		}
		std::stringstream strGeometryStream;
		strGeometryStream << geometryStream.rdbuf();
		geometryStream.close();

		tmpSouce = strGeometryStream.str(); // 必须先转成std::string类型, 再c_str, 否则乱码
		const char* geometrySource = tmpSouce.c_str();

		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometrySource, NULL);
		glCompileShader(geometryShader);

		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
			std::cout << "geometry shader compile fail. reason: " << infoLog << std::endl;
		}
	}
	
	// compile fragment shader
	std::fstream fragmentStream(fragmentPath, std::ios_base::in);
	if (!fragmentStream.is_open()) {
		std::cout << "Open fragment shader file fail. path: " << fragmentPath << std::endl;
	}

	std::stringstream strFragmentStream;
	strFragmentStream << fragmentStream.rdbuf();
	fragmentStream.close();

	tmpSouce = strFragmentStream.str(); // 必须先转成std::string类型, 再c_str, 否则乱码
	const char* fragmentSource = tmpSouce.c_str();

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "fragment shader compile fail. reason: " << infoLog << std::endl;
	}

	// link shader program
	m_id = glCreateProgram();
	glAttachShader(m_id, vertexShader);
	if (geometryPath) {
		glAttachShader(m_id, geometryShader);
	}
	glAttachShader(m_id, fragmentShader);
	glLinkProgram(m_id);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

AbstractShader::~AbstractShader()
{
	glDeleteShader(m_id);
}

void AbstractShader::setBool(const std::string& name, bool value)
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void AbstractShader::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void AbstractShader::setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void AbstractShader::setFloat(const std::string& name, float v1, float v2, float v3, float v4)
{
	glUniform4f(glGetUniformLocation(m_id, name.c_str()), v1, v2, v3, v4);
}

void AbstractShader::setVec(const std::string & name, float v1, float v2)
{
	glUniform2f(glGetUniformLocation(m_id, name.c_str()), v1, v2);
}

void AbstractShader::setVec(const std::string& name, float v1, float v2, float v3)
{
	glUniform3f(glGetUniformLocation(m_id, name.c_str()), v1, v2, v3);
}

void AbstractShader::setVec(const std::string & name, float v1, float v2, float v3, float v4)
{
	glUniform4f(glGetUniformLocation(m_id, name.c_str()), v1, v2, v3, v4);
}

void AbstractShader::setVec(const std::string & name, glm::vec2 vec)
{
	setVec(name, vec.x, vec.y);
}

void AbstractShader::setVec(const std::string& name, glm::vec3 vec)
{
	setVec(name, vec.x, vec.y, vec.z);
}

void AbstractShader::setVec(const std::string& name, glm::vec4 vec)
{
	setVec(name, vec.x, vec.y, vec.z, vec.w);
}

void AbstractShader::setMatrix(const std::string& name, float* value)
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, value);
}

void AbstractShader::setMatrix(const std::string& name, glm::mat3 value)
{
	glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void AbstractShader::setMatrix(const std::string& name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void AbstractShader::setUniformBlockBinding(const std::string& name, unsigned int value)
{
	glUniformBlockBinding(m_id, glGetUniformBlockIndex(m_id, name.c_str()), value);
}

SimpleShader::SimpleShader(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
	: AbstractShader(vertexPath, geometryPath, fragmentPath)
{
}

void SimpleShader::use()
{
	glUseProgram(m_id);
}
