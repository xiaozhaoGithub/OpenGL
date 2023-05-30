#ifndef SHADERFACTORY_H_
#define SHADERFACTORY_H_

#include <string>
#include <map>

#include "Shader.h"

// flyweight pattern 
class ShaderFactory
{
public:
	std::shared_ptr<AbstractShader> shaderProgram(const std::string& key, const char* vertexPath, const char* fragmentPath);
	std::shared_ptr<AbstractShader> shaderProgram(const std::string& key, const char* vertexPath, const char* geometryPath, const char* fragmentPath);

private:
	std::map<std::string, std::shared_ptr<AbstractShader> > m_map;
};

#endif

