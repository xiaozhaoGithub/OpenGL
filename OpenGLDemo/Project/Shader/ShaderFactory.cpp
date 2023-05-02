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
		m_map[key] = std::make_shared<SimpleShader>(vertexPath, fragmentPath);
	}

	return m_map[key];
}
