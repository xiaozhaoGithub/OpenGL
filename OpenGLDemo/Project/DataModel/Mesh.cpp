#include "Mesh.h"

#include <iostream>

#include "glad/glad.h"

Mesh::Mesh(const std::vector<Vertex>& vertexs, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
{
	m_vertexs = vertexs;
	m_indices = indices;
	m_textures = textures;

	initMesh();
}

Mesh::Mesh(const VertexsData& vertexs, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
{
	m_vertexsData = vertexs;
	m_indices = indices;
	m_textures = textures;

	initMesh();
}

void Mesh::initMesh()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

#ifdef BATCH_VERTEXT_ATTR
	// 分配实际内存大小及偏移量
	size_t positionsBufSize = sizeof(glm::vec3) * m_vertexsData.positions.size();
	size_t normalsBufSize = sizeof(glm::vec3) * m_vertexsData.normals.size();
	size_t texCoordsBufSize = sizeof(glm::vec2) * m_vertexsData.texCoords.size();
	glBufferData(GL_ARRAY_BUFFER, positionsBufSize + normalsBufSize + texCoordsBufSize, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, positionsBufSize, &m_vertexsData.positions[0]);
	glBufferSubData(GL_ARRAY_BUFFER, positionsBufSize, normalsBufSize, &m_vertexsData.normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, positionsBufSize + normalsBufSize, texCoordsBufSize, &m_vertexsData.texCoords[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(positionsBufSize));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(positionsBufSize + normalsBufSize));
#else
	// 从第一个元素的地址开始
	glBufferData(GL_ARRAY_BUFFER, m_vertexs.size() * sizeof(Vertex), &m_vertexs[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	int offset = offsetof(Vertex, normal);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	offset = offsetof(Vertex, texCoords);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoords)));
#endif
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::draw(std::shared_ptr<AbstractShader> shader)
{
	unsigned int diffuseNum = 1;
	unsigned int specularNum = 1;
	unsigned int normalNum = 1;
	unsigned int heightNum = 1;
	unsigned int reflectionNum = 1;

	for (unsigned int i = 0; i < m_textures.size(); ++i) {
		std::string textureName;
		auto type = m_textures[i].type;
		if (type == "diffuse") {
			textureName = type + std::to_string(diffuseNum++);
		}
		else if (type == "specular") {
			textureName = type + std::to_string(specularNum++);
		}
		else if (type == "texture_normal") {
			textureName = type + std::to_string(normalNum++);
		}
		else if (type == "texture_height") {
			textureName = type + std::to_string(heightNum++);
		}
		else if (type == "texture_reflection") {
			textureName = type + std::to_string(reflectionNum++);
		}

		std::string name = "material." + textureName;
		shader->setInt(name, i);

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// 绘制网格
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
