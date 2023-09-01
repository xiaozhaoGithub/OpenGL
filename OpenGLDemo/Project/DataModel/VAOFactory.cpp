#include "VAOFactory.h"

#include <iostream>
#include <memory>

#include "stb_image.h"

#include "CommonDataDef.h"
#include "TextureHelper.h"

using namespace TextureHelper;

namespace DataDef
{
	// 深度冲突问题解决: 通过在两个物体之间设置一个用户无法注意到的偏移值（立方体与地板相聚0.01f）
	// 定义面剔除（逆时针为正向面）
	const float cubeVertices[] = {
		// Back face
		-0.5f, -0.49f, -0.5f,  0.0f, 0.0f, // Bottom-left
		0.5f,  0.51f, -0.5f,  1.0f, 1.0f, // top-right
		0.5f, -0.49f, -0.5f,  1.0f, 0.0f, // bottom-right         
		0.5f,  0.51f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.49f, -0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.51f, -0.5f,  0.0f, 1.0f, // top-left
		// Front face
		-0.5f, -0.49f,  0.5f,  0.0f, 0.0f, // bottom-left
		0.5f, -0.49f,  0.5f,  1.0f, 0.0f, // bottom-right
		0.5f,  0.51f,  0.5f,  1.0f, 1.0f, // top-right
		0.5f,  0.51f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.51f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.51f,  0.5f,  0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f,  0.51f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.51f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.49f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.49f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.49f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.51f,  0.5f,  1.0f, 0.0f, // top-right
		// Right face
		0.5f,  0.51f,  0.5f,  1.0f, 0.0f, // top-left
		0.5f, -0.49f, -0.5f,  0.0f, 1.0f, // bottom-right
		0.5f,  0.51f, -0.5f,  1.0f, 1.0f, // top-right         
		0.5f, -0.49f, -0.5f,  0.0f, 1.0f, // bottom-right
		0.5f,  0.51f,  0.5f,  1.0f, 0.0f, // top-left
		0.5f, -0.49f,  0.5f,  0.0f, 0.0f, // bottom-left     
		// Bottom face
		-0.5f, -0.49f, -0.5f,  0.0f, 1.0f, // top-right
		0.5f, -0.49f, -0.5f,  1.0f, 1.0f, // top-left
		0.5f, -0.49f,  0.5f,  1.0f, 0.0f, // bottom-left
		0.5f, -0.49f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.49f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f, -0.49f, -0.5f,  0.0f, 1.0f, // top-right
		// Top face
		-0.5f,  0.51f, -0.5f,  0.0f, 1.0f, // top-left
		0.5f,  0.51f,  0.5f,  1.0f, 0.0f, // bottom-right
		0.5f,  0.51f, -0.5f,  1.0f, 1.0f, // top-right     
		0.5f,  0.51f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.51f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f,  0.51f,  0.5f,  0.0f, 0.0f  // bottom-left      
	};

	float cubeNormalVertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};

	const float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	const float floorVertices[] = {
		// positions          //normal		    // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,	2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f, 0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f, 2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f, 0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f, 2.0f, 2.0f
	};

	const float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	float quadColorVertices[] = {
		// 位置          // 颜色
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};
}
namespace DD = DataDef;

void AbstractVAO::bindVAO()
{
	glBindVertexArray(m_id);
}

void AbstractVAO::insertTexture(unsigned int type, unsigned int id)
{
	m_textures.push_back(std::make_pair(type, id));
}

void AbstractVAO::bindTexture()
{
	for (unsigned int i = 0; i < m_textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(m_textures[i].first, m_textures[i].second);
	}
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createNormalVAO()
{
	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	// 可创建不同的VAO进行切换
	// 绘制多个物体时，你首先要生成/配置所有的VAO（和必须的VBO及属性指针)，然后储存它们供后面使用
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData是一个专门用来把用户定义的顶点数据复制到当前绑定缓冲的函数。
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 告诉OpenGL该如何解析顶点数据（应用到逐个顶点属性上）
	// 绑定的VAO保存顶点属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。

	glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑VBO

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

unsigned int TriangleVAOFactory::createSpecialVAO()
{
	float nextVertices[] = {
	-1.0f, -1.0f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int nextVBO;
	glGenBuffers(1, &nextVBO);
	glBindBuffer(GL_ARRAY_BUFFER, nextVBO);
	// glBufferData是一个专门用来把用户定义的顶点数据复制到当前绑定缓冲的函数。
	glBufferData(GL_ARRAY_BUFFER, sizeof(nextVertices), nextVertices, GL_STATIC_DRAW);

	// 告诉OpenGL该如何解析顶点数据（应用到逐个顶点属性上）
	// 绑定的VAO保存顶点属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。

	glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑VBO

	return VAO;
}

unsigned int TriangleVAOFactory::createAdjacentVAO()
{
	float vertics[] = {
		-1.0f, 0.0f, 0.0f,
		-0.5f, 1.0f, 0.0f,
		-0.0f, 0.0f, 0.0f,

		-0.0f, 0.0f, 0.0f,
		0.5f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertics), vertics, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // 一般不用主动解绑，而是下次调用时，主动绑定新的VAO

	return VAO;
}

std::pair<unsigned int, unsigned int> TriangleVAOFactory::createAdjacent2VAO()
{
	float verticsA[] = {
		-1.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f
	};

	unsigned int VAO[2];
	glGenVertexArrays(2, VAO);
	glBindVertexArray(VAO[0]);

	unsigned int VBO[2];
	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticsA), verticsA, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	float verticsB[] = {
	0.0f, -1.0f, 0.0f,
	0.5f, 0.0f, 0.0f,
	1.0f, -1.0f, 0.0f
	};

	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticsB), verticsB, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	return std::make_pair(VAO[0], VAO[1]);
}

unsigned int TriangleVAOFactory::createRichVAO()
{
	float verticesWithColor[] = {
		-0.5f, -0.5f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f,  0.5f, 0.0f,
		0.0f, 0.0f, 1.0f,
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesWithColor), verticesWithColor, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。
	glEnableVertexAttribArray(1); // 顶点属性位置值(location = 1)作为参数，启用顶点属性；顶点属性默认是禁用的。

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::creatTextureVAO()
{
	float vertices[] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	// GL_REPEAT
	//float vertices[] = {
	//	// positions          // colors           // texture coords
	//	0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f, // top right
	//	0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f, // bottom right
	//	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	//	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f  // top left 
	//};

	//// GL_NEAREST
	//float vertices[] = {
	//	// positions          // colors           // texture coords (note that we changed them to 'zoom in' on our texture image)
	//	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.55f, 0.55f, // top right
	//	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.55f, 0.45f, // bottom right
	//	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.45f, 0.45f, // bottom left
	//	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.45f, 0.55f  // top left 
	//};

	unsigned int indices[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
	};


	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1); // 顶点属性位置值(location = 1)作为参数，启用顶点属性；顶点属性默认是禁用的。

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2); // 顶点属性位置值(location = 2)作为参数，启用顶点属性；顶点属性默认是禁用的。

	// 生成并绑定纹理
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// 为当前绑定的纹理对象设置环绕、过滤方式
	// 纹理环绕
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // default
	// 纹理过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 多级渐远纹理
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 多级纹理间线性插值

	int width;
	int height;
	int colorChls;
	stbi_uc* pixels = stbi_load("skin/container.jpg", &width, &height, &colorChls, 0);
	if (pixels) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		// 在生成纹理之后调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(pixels);

	std::shared_ptr<AbstractVAO> abstractVAO = std::make_shared<TriangleVAO>(VAO);
	abstractVAO->insertTexture(GL_TEXTURE_2D, texture1);

	return abstractVAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::creatMixTextureUnitVAO()
{
	std::shared_ptr<AbstractVAO> abstractVAO = creatTextureVAO();

	unsigned int texture2; 
	glGenTextures(1, &texture2);

	glActiveTexture(GL_TEXTURE1); // 着色器需要对纹理单元设置位置（1），此处也需要将纹理对象绑定到对应的纹理单元（GL_TEXTURE1）
	glBindTexture(GL_TEXTURE_2D, texture2);// 纹理对象与VAO是否绑定没有关系，在渲染时，需要绑定每个纹理单元的纹理对象引用，就像渲染时绑定VAO一样
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // 默认超出纹理坐标（通常为(0,0)到(1,1)），则重复渲染图片
	//float colors[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colors);

	// 纹理过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int w, h, chls;
	stbi_set_flip_vertically_on_load(true);
	auto pixels = stbi_load("skin/awesomeface.png", &w, &h, &chls, 0);
	if (pixels) {
		unsigned int format = chls == 4 ? GL_RGBA : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pixels);
		// 在生成纹理之后调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(pixels);

	abstractVAO->insertTexture(GL_TEXTURE_2D, texture2);

	return abstractVAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createTransformVAO()
{
	auto VAO = creatMixTextureUnitVAO();
	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::create3DVAO()
{
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1); // 顶点属性位置值(location = 1)作为参数，启用顶点属性；顶点属性默认是禁用的。

	// 生成并绑定纹理
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// 为当前绑定的纹理对象设置环绕、过滤方式
	// 纹理环绕
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // default
	// 纹理过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 多级渐远纹理
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// 多级纹理间线性插值

	int width;
	int height;
	int colorChls;
	auto pixels = stbi_load("skin/container.jpg", &width, &height, &colorChls, 0);
	if (pixels) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		// 在生成纹理之后调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(pixels);

	unsigned int texture2;
	glGenTextures(1, &texture2);

	glActiveTexture(GL_TEXTURE1); // 着色器需要对纹理单元设置位置（1），此处也需要将纹理对象绑定到对应的纹理单元（GL_TEXTURE1）
	glBindTexture(GL_TEXTURE_2D, texture2);// 纹理对象与VAO是否绑定没有关系，在渲染时，需要绑定每个纹理单元的纹理对象引用，就像渲染时绑定VAO一样

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // 默认超出纹理坐标（通常为(0,0)到(1,1)），则重复渲染图片
	//float colors[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colors);

	// 纹理过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int w, h, chls;
	stbi_set_flip_vertically_on_load(true);
	pixels = stbi_load("skin/awesomeface.png", &w, &h, &chls, 0);
	if (pixels) {
		unsigned int format = chls == 4 ? GL_RGBA : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pixels);
		// 在生成纹理之后调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(pixels);

	std::shared_ptr<AbstractVAO> abstractVAO = std::make_shared<TriangleVAO>(VAO);
	abstractVAO->insertTexture(GL_TEXTURE_2D, texture1);
	abstractVAO->insertTexture(GL_TEXTURE_2D, texture2);

	return abstractVAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createLightVAO()
{
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createTargetVAO()
{
	auto VAO = createLightVAO();

	// 常见问题：重置顶点属性指针时，第一个参数设置了错误的位置导致崩溃
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);// 法向量location

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createLightMapTargetVAO()
{
	auto VAO = createTargetVAO();
	
	// note：加载纹理前必须先指定要操作的纹理位置，否则显示错乱，不对应
	glActiveTexture(GL_TEXTURE0); // 着色器需要对纹理单元设置位置（0），此处也需要将纹理对象绑定到对应的纹理单元（GL_TEXTURE0）
	unsigned int textureDiffuseMap = loadTexture("skin/container2.png");

	glActiveTexture(GL_TEXTURE1); // 着色器需要对纹理单元设置位置（0），此处也需要将纹理对象绑定到对应的纹理单元（GL_TEXTURE0）
	unsigned int textureSpecularMap = loadTexture("skin/container2_specular.png");
	//unsigned int textureSpecular = loadTexture("skin/lighting_maps_specular_color.png");

	glActiveTexture(GL_TEXTURE2); // 着色器需要对纹理单元设置位置（0），此处也需要将纹理对象绑定到对应的纹理单元（GL_TEXTURE0）
	unsigned int textureEmissionMap = loadTexture("skin/matrix.jpg");

	VAO->insertTexture(GL_TEXTURE_2D, textureDiffuseMap);
	VAO->insertTexture(GL_TEXTURE_2D, textureSpecularMap);
	VAO->insertTexture(GL_TEXTURE_2D, textureEmissionMap);

	// 常见问题：重置顶点属性指针时，第一个location参数设置了错误的位置导致崩溃
	// 常见问题：重置顶点属性指针时，第一个size参数与实际的不一致导致部分渲染戳错（实际大小为2，实际填3导致顶部一个三角形未渲染）
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createCubeVAO()
{
	// 复制缓冲接口测试, 先生成源buffer
	unsigned int srcVBO;
	glGenBuffers(1, &srcVBO);
	glBindBuffer(GL_ARRAY_BUFFER, srcVBO);

	// 三种填充顶点数据Buffer的方式
	// way 1
	glBufferData(GL_ARRAY_BUFFER, sizeof(DD::cubeVertices), DD::cubeVertices, GL_STATIC_DRAW);

	// way 2
	//glBufferData(GL_ARRAY_BUFFER, sizeof(DD::cubeVertices), nullptr, GL_STATIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DD::cubeVertices), DD::cubeVertices);

	// way 3
	// 如果要直接映射数据到缓冲，而不事先将其存储到临时内存中，glMapBuffer这个函数会很有用
	//glBufferData(GL_ARRAY_BUFFER, sizeof(DD::cubeVertices), nullptr, GL_STATIC_DRAW);
	//void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	//memcpy(ptr, DD::cubeVertices, sizeof(DD::cubeVertices));
	//// 告诉OpenGL我们不再需要这个指针了
	//GLboolean ret = glUnmapBuffer(GL_ARRAY_BUFFER);
	//if (ret == GL_FALSE) {
	//	std::cout << "vertex data map failed." << std::endl;
	//}

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DD::cubeVertices), nullptr, GL_STATIC_DRAW); //必须分配内存

	// 复制缓冲
	glBindBuffer(GL_COPY_READ_BUFFER, srcVBO);
	glBindBuffer(GL_COPY_WRITE_BUFFER, VBO);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(DD::cubeVertices));
	//glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(DD::cubeVertices));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));

	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。
	glEnableVertexAttribArray(1);

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createNormalCubeVAO()
{
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DD::cubeNormalVertices), DD::cubeNormalVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createAdvancedTargetVAO()
{
	auto VAO = createCubeVAO();
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/textures/marble.jpg"));

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createTexCubeVAO()
{
	auto VAO = createNormalCubeVAO();
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/textures/wood.png"));

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createPlaneVAO()
{
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DD::planeVertices), DD::planeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createPlaneVAO(char const* path)
{
	auto VAO = createPlaneVAO();
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture(path));

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createVPlaneVAO()
{
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DD::transparentVertices), DD::transparentVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createFloorVAO()
{
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DD::floorVertices), DD::floorVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createFloorVAO(char const* path)
{
	auto VAO = createFloorVAO();

	// 因为不是所有纹理都是在sRGB空间中的所以当你把纹理指定为sRGB纹理时要格外小心。
	// 比如diffuse纹理，这种为物体上色的纹理几乎都是在sRGB空间中的。
	// 而为了获取光照参数的纹理，像specular贴图和法线贴图几乎都在线性空间中，所以如果你把它们也配置为sRGB纹理的话，光照就坏掉了。指定sRGB纹理时要当心。
	TexParam param;
	param.internalFormat3 = GL_SRGB;
	param.internalFormat4 = GL_SRGB_ALPHA;

	// 指定为以上两种内部sRGB纹理格式其中之一，OpenGL将自动把颜色校正到线性空间中
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture(path, param));

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createVegetationVAO()
{
	auto VAO = createVPlaneVAO();

	TexParam param;
	param.wrapS = GL_CLAMP_TO_EDGE;
	param.wrapT = GL_CLAMP_TO_EDGE;

	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/textures/grass.png", param));

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createWindowVAO()
{
	auto VAO = createVPlaneVAO();
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/textures/window.png"));

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createQuadVAO()
{
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DD::quadVertices), DD::quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createQuadColorVAO()
{
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DD::quadColorVertices), DD::quadColorVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 2));

	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。
	glEnableVertexAttribArray(1);

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createPostProcessVAO()
{
	auto VAO = createCubeVAO();
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/container.jpg"));

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createCubeMapVAO()
{
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createSkyboxVAO()
{
	auto VAO = createCubeMapVAO();

	std::vector<std::string> faces = {
		"skin/textures/skybox/right.jpg",
		"skin/textures/skybox/left.jpg",
		"skin/textures/skybox/top.jpg",
		"skin/textures/skybox/bottom.jpg",
		"skin/textures/skybox/front.jpg",
		"skin/textures/skybox/back.jpg"
	};

	unsigned int texId = loadCubemap(faces);
	VAO->insertTexture(GL_TEXTURE_CUBE_MAP, texId);

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createRelectedCubeVAO()
{
	auto VAO = createTargetVAO();

	std::vector<std::string> faces = {
	"skin/textures/skybox/right.jpg",
	"skin/textures/skybox/left.jpg",
	"skin/textures/skybox/top.jpg",
	"skin/textures/skybox/bottom.jpg",
	"skin/textures/skybox/front.jpg",
	"skin/textures/skybox/back.jpg"
	};

	unsigned int texId = loadCubemap(faces);
	VAO->insertTexture(GL_TEXTURE_CUBE_MAP, texId);

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createPointVAO()
{
	float vertices[] = {
		0.0f, 0.0f, 0.0f
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0); 

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createPointsVAO()
{
	float vertices[] = {
		-4.0f, -0.3f, 4.0f, 1.0f, 0.0f, 0.0f,
		-4.0f, 0.3f, 4.0f, 0.0f, 1.0f, 0.0f,
		-3.5f, -0.3f, 4.0f, 0.0f, 0.0f, 1.0f
		-3.5f, 0.3f, 4.0f, 1.0f, 1.0f, 0.0f 
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createInstancedVAO()
{
	auto VAO = createQuadColorVAO();

	glm::vec2 translations[100];
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2) {
		for (int x = -10; x < 10; x += 2) {
			glm::vec2 translation;
			translation.x = (float)x / 10.0f + offset;
			translation.y = (float)y / 10.0f + offset;
			translations[index++] = translation;
		}
	}

	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, 100 * sizeof(glm::vec2), &translations[0], GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// 告诉了OpenGL该什么时候更新顶点属性的内容至新一组数据
	glVertexAttribDivisor(2, 1); // 必须指定渲染更新实例（希望在渲染"1个"新实例的时候更新顶点属性）

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createTangentVAO()
{
	// positions
	glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
	glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
	glm::vec3 pos3(1.0f, -1.0f, 0.0f);
	glm::vec3 pos4(1.0f, 1.0f, 0.0f);
	// texture coordinates
	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 uv4(1.0f, 1.0f);
	// normal vector
	glm::vec3 nm(0.0f, 0.0f, 1.0f);

	// calculate tangent/bitangent vectors of both triangles
	glm::vec3 tangent1, bitangent1;
	glm::vec3 tangent2, bitangent2;
	// triangle 1
	// ----------
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

	// triangle 2
	// ----------
	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


	bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


	float quadVertices[] = {
		// positions            // normal         // texcoords  // tangent                          // bitangent
		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

		pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
	};
	// configure plane VAO
	unsigned int quadVAO;
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	unsigned int quadVBO;
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(quadVAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createNormalMapVAO()
{
	auto VAO = createTangentVAO();
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/textures/brickwall.jpg"));
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/textures/brickwall_normal.jpg"));

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createParallaxMapVAO()
{
	auto VAO = createTangentVAO();
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/textures/bricks2.jpg"));
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/textures/bricks2_normal.jpg"));
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/textures/bricks2_disp.jpg"));

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createPbrLightingVAO()
{
	// 构造球体模型
	unsigned int sphereVAO;
	glGenVertexArrays(1, &sphereVAO);

	unsigned int vbo, ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;
	const float PI = 3.14159265359f;
	for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			positions.push_back(glm::vec3(xPos, yPos, zPos));
			normals.push_back(glm::vec3(xPos, yPos, zPos));
			uv.push_back(glm::vec2(xSegment, ySegment));
		}
	}

	bool oddRow = false;
	for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
		// even rows: y == 0, y == 2; and so on 
		if (!oddRow)  {
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
				indices.push_back(y       * (X_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else {
			for (int x = X_SEGMENTS; x >= 0; --x) {
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				indices.push_back(y       * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}

	unsigned int indexCount = static_cast<unsigned int>(indices.size());

	std::vector<float> data;
	for (unsigned int i = 0; i < positions.size(); ++i) {
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);
		if (normals.size() > 0)
		{
			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);
		}
		if (uv.size() > 0)
		{
			data.push_back(uv[i].x);
			data.push_back(uv[i].y);
		}
	}
	glBindVertexArray(sphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	unsigned int stride = (3 + 3 + 2) * sizeof(float);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

	return std::shared_ptr<AbstractVAO>(new ElementVAO(sphereVAO, indexCount));
}

std::shared_ptr<AbstractVAO> RectVAOFactory::createNormalVAO()
{
	float rectVertices[] = {
		0.5f, 0.5f, 0.0f, 
		0.5f, -0.5f, 0.0f,  
		-0.5f, -0.5f, 0.0f, 
		-0.5f, 0.5f, 0.0f   
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);

	// 元素(索引)缓冲区，EBO
	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	// 意味着我们每次想要使用索引渲染对象时都必须绑定相应的EBO，这又有点麻烦。
	// 碰巧顶点数组对象也跟踪元素缓冲区对象绑定。在绑定VAO时，绑定的最后一个元素缓冲区对象存储为VAO的元素缓冲区对象。
	// 然后，绑定到VAO也会自动绑定该EBO。
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}
