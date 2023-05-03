#ifndef MESH_H_
#define MESH_H_

#include <string>
#include <vector>

#include "glm.hpp"
#include "Shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	unsigned int id;
	std::string name;
};

/**
 * @brief ģ�ͻ��ƣ�������Ϊ��λ���л��ƣ�һ���������������㡢��������������
 */
class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertexs, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);

	void setupMesh();
	void draw(std::shared_ptr<AbstractShader> shader);

private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	std::vector<Vertex> m_vertexs;
	std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;
};

#endif

