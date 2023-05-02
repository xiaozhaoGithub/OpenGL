#ifndef SHADER_H_
#define SHADER_H_

#include <string>

#include "gtc/type_ptr.hpp"

class AbstractShader
{
public:
	AbstractShader(const char* vertexPath, const char* fragmentPath) : m_id(0) {}
	virtual ~AbstractShader();

	virtual void use() = 0;

	void setBool(const std::string& name, bool value);

	void setInt(const std::string& name, int value);

	void setFloat(const std::string& name, float value);
	void setFloat(const std::string& name, float v1, float v2, float v3, float v4);

	void setVec(const std::string& name, float v1, float v2, float v3);
	void setVec(const std::string& name, float v1, float v2, float v3, float v4);
	void setVec(const std::string& name, glm::vec3 vec);
	void setVec(const std::string& name, glm::vec4 vec);

	void setMatrix(const std::string& name, float* value);

protected:
	unsigned int m_id;
};

// 编译顶点着色器和片段着色器（几何着色器可选）
class SimpleShader : public AbstractShader
{
public:
	SimpleShader(const char* vertexPath, const char* fragmentPath);
	void use() override;
};

#endif

