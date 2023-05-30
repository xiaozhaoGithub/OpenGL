#include "Shader.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "glad/glad.h"
#include "ShaderFactory.h"

std::shared_ptr<AbstractShader> ShaderFactory::shaderProgram(const std::string& key, const char* vertexPath, const char* fragmentPath)
{
	auto iter = m_map.find(key);
	if (iter == m_map.end()) {
		m_map[key] = std::make_shared<SimpleShader>(vertexPath, nullptr, fragmentPath);
	}

	return m_map[key];
}

std::shared_ptr<AbstractShader> ShaderFactory::shaderProgram(const std::string& key, const char* vertexPath, const char* geometryPath, const char* fragmentPath)
{
	auto iter = m_map.find(key);
	if (iter == m_map.end()) {
		m_map[key] = std::make_shared<SimpleShader>(vertexPath, geometryPath, fragmentPath);
	}

	return m_map[key];
}
