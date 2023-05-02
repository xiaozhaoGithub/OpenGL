#include "Mesh.h"

#include "glad/glad.h"

Mesh::Mesh(const std::vector<Vertex>& vertexs, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
{
	m_vertexs = vertexs;
	m_indices = indices;
	m_textures = textures;

	setupMesh();
}

Mesh::~Mesh()
{
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 从第一个元素的地址开始
	glBufferData(GL_ARRAY_BUFFER, m_vertexs.size() * sizeof(Vertex), &m_vertexs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	int offset = offsetof(Vertex, normal);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);

	offset = offsetof(Vertex, texCoords);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoords)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::draw(std::shared_ptr<AbstractShader> shader)
{
	unsigned int diffuseNum = 1;
	unsigned int specularNum = 1;

	for (unsigned int i = 0; i < m_textures.size(); ++i) {
		std::string textureName;
		auto name = m_textures[i].name;
		if (name == "diffuse") {
			textureName = name + std::to_string(diffuseNum++);
		}
		else if (name == "specular") {
			textureName = name + std::to_string(specularNum++);
		}

		shader->setInt("material." + name, 3 + i);

		glActiveTexture(GL_TEXTURE3 + i); //特别注意，纹理枚举需要根据实际情况考虑，若此设置为GL_TEXTURE0，若纹理对象非重刷新，则此处纹理替代
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// 绘制网格
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
