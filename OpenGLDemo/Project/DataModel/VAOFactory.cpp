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
		0.5f, -0.51f,  0.5f,  1.0f, 0.0f, // bottom-left
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

	const float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
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
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(DD::cubeVertices), DD::cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // 顶点属性位置值(location = 0)作为参数，启用顶点属性；顶点属性默认是禁用的。

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createAdvancedTargetVAO()
{
	auto VAO = createCubeVAO();
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/textures/marble.jpg"));

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
	auto VAO = createPlaneVAO();
	VAO->insertTexture(GL_TEXTURE_2D, loadTexture("skin/textures/metal.png"));

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
