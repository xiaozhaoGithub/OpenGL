#include "VAOFactory.h"

#include <iostream>
#include <memory>

#include "glad/glad.h"
// ��ȷ�����ڰ���GLFW��ͷ�ļ�֮ǰ������GLAD��ͷ�ļ���GLAD��ͷ�ļ���������ȷ��OpenGLͷ�ļ�������GL/gl.h����
// ������Ҫ������������OpenGL��ͷ�ļ�֮ǰ����GLAD��
#include "stb_image.h"

void AbstractVAO::bindVAO()
{
	glBindVertexArray(m_id);
}

void AbstractVAO::bindTexture()
{
	for (auto pair : m_indexTextureMap) {
		glActiveTexture(GL_TEXTURE0 + pair.first);
		glBindTexture(GL_TEXTURE0 + pair.first, pair.second);
	}
}

unsigned int AbstractVAOFactory::loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); // ����������֮�����glGenerateMipmap�����Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}

	stbi_image_free(data);

	return textureID;
}

unsigned int TriangleVAOFactory::createNormalVAO()
{
	float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	// �ɴ�����ͬ��VAO�����л�
	// ���ƶ������ʱ��������Ҫ����/�������е�VAO���ͱ����VBO������ָ��)��Ȼ�󴢴����ǹ�����ʹ��
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData��һ��ר���������û�����Ķ������ݸ��Ƶ���ǰ�󶨻���ĺ�����
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// ����OpenGL����ν����������ݣ�Ӧ�õ�������������ϣ�
	// �󶨵�VAO���涥������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // ��������λ��ֵ(location = 0)��Ϊ���������ö������ԣ���������Ĭ���ǽ��õġ�

	glBindBuffer(GL_ARRAY_BUFFER, 0); // ���VBO

	return VAO;
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
	// glBufferData��һ��ר���������û�����Ķ������ݸ��Ƶ���ǰ�󶨻���ĺ�����
	glBufferData(GL_ARRAY_BUFFER, sizeof(nextVertices), nextVertices, GL_STATIC_DRAW);

	// ����OpenGL����ν����������ݣ�Ӧ�õ�������������ϣ�
	// �󶨵�VAO���涥������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // ��������λ��ֵ(location = 0)��Ϊ���������ö������ԣ���������Ĭ���ǽ��õġ�

	glBindBuffer(GL_ARRAY_BUFFER, 0); // ���VBO

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

	glBindVertexArray(0); // һ�㲻��������󣬶����´ε���ʱ���������µ�VAO

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
	glEnableVertexAttribArray(0); // ��������λ��ֵ(location = 0)��Ϊ���������ö������ԣ���������Ĭ���ǽ��õġ�
	glEnableVertexAttribArray(1); // ��������λ��ֵ(location = 1)��Ϊ���������ö������ԣ���������Ĭ���ǽ��õġ�

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
	glEnableVertexAttribArray(0); // ��������λ��ֵ(location = 0)��Ϊ���������ö������ԣ���������Ĭ���ǽ��õġ�

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1); // ��������λ��ֵ(location = 1)��Ϊ���������ö������ԣ���������Ĭ���ǽ��õġ�

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2); // ��������λ��ֵ(location = 2)��Ϊ���������ö������ԣ���������Ĭ���ǽ��õġ�

	// ���ɲ�������
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
	// ������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // default
	// �������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// �༶��Զ����
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �༶��������Բ�ֵ

	int width;
	int height;
	int colorChls;
	stbi_uc* pixels = stbi_load("skin/container.jpg", &width, &height, &colorChls, 0);
	if (pixels) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		// ����������֮�����glGenerateMipmap�����Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(pixels);

	std::shared_ptr<AbstractVAO> abstractVAO = std::make_shared<TriangleVAO>(VAO);
	abstractVAO->insertTexture(0, texture1);

	return abstractVAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::creatMixTextureUnitVAO()
{
	std::shared_ptr<AbstractVAO> abstractVAO = creatTextureVAO();

	unsigned int texture2; 
	glGenTextures(1, &texture2);

	glActiveTexture(GL_TEXTURE1); // ��ɫ����Ҫ������Ԫ����λ�ã�1�����˴�Ҳ��Ҫ���������󶨵���Ӧ������Ԫ��GL_TEXTURE1��
	glBindTexture(GL_TEXTURE_2D, texture2);// ���������VAO�Ƿ��û�й�ϵ������Ⱦʱ����Ҫ��ÿ������Ԫ������������ã�������Ⱦʱ��VAOһ��
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Ĭ�ϳ����������꣨ͨ��Ϊ(0,0)��(1,1)�������ظ���ȾͼƬ
	//float colors[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colors);

	// �������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int w, h, chls;
	stbi_set_flip_vertically_on_load(true);
	auto pixels = stbi_load("skin/awesomeface.png", &w, &h, &chls, 0);
	if (pixels) {
		unsigned int format = chls == 4 ? GL_RGBA : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pixels);
		// ����������֮�����glGenerateMipmap�����Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(pixels);

	abstractVAO->insertTexture(1, texture2);

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
	glEnableVertexAttribArray(0); // ��������λ��ֵ(location = 0)��Ϊ���������ö������ԣ���������Ĭ���ǽ��õġ�

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1); // ��������λ��ֵ(location = 1)��Ϊ���������ö������ԣ���������Ĭ���ǽ��õġ�

	// ���ɲ�������
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
	// ������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // default
	// �������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// �༶��Զ����
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);// �༶��������Բ�ֵ

	int width;
	int height;
	int colorChls;
	auto pixels = stbi_load("skin/container.jpg", &width, &height, &colorChls, 0);
	if (pixels) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		// ����������֮�����glGenerateMipmap�����Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(pixels);

	unsigned int texture2;
	glGenTextures(1, &texture2);

	glActiveTexture(GL_TEXTURE1); // ��ɫ����Ҫ������Ԫ����λ�ã�1�����˴�Ҳ��Ҫ���������󶨵���Ӧ������Ԫ��GL_TEXTURE1��
	glBindTexture(GL_TEXTURE_2D, texture2);// ���������VAO�Ƿ��û�й�ϵ������Ⱦʱ����Ҫ��ÿ������Ԫ������������ã�������Ⱦʱ��VAOһ��

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Ĭ�ϳ����������꣨ͨ��Ϊ(0,0)��(1,1)�������ظ���ȾͼƬ
	//float colors[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colors);

	// �������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int w, h, chls;
	stbi_set_flip_vertically_on_load(true);
	pixels = stbi_load("skin/awesomeface.png", &w, &h, &chls, 0);
	if (pixels) {
		unsigned int format = chls == 4 ? GL_RGBA : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pixels);
		// ����������֮�����glGenerateMipmap�����Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(pixels);

	std::shared_ptr<AbstractVAO> abstractVAO = std::make_shared<TriangleVAO>(VAO);
	abstractVAO->insertTexture(0, texture1);
	abstractVAO->insertTexture(1, texture2);

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
	glEnableVertexAttribArray(0); // ��������λ��ֵ(location = 0)��Ϊ���������ö������ԣ���������Ĭ���ǽ��õġ�

	return std::shared_ptr<AbstractVAO>(new TriangleVAO(VAO));
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createTargetVAO()
{
	auto VAO = createLightVAO();

	// �������⣺���ö�������ָ��ʱ����һ�����������˴����λ�õ��±���
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);// ������location

	return VAO;
}

std::shared_ptr<AbstractVAO> TriangleVAOFactory::createLightMapTargetVAO()
{
	auto VAO = createTargetVAO();
	
	// note����������ǰ������ָ��Ҫ����������λ�ã�������ʾ���ң�����Ӧ
	glActiveTexture(GL_TEXTURE0); // ��ɫ����Ҫ������Ԫ����λ�ã�0�����˴�Ҳ��Ҫ���������󶨵���Ӧ������Ԫ��GL_TEXTURE0��
	unsigned int textureDiffuseMap = loadTexture("skin/container2.png");

	glActiveTexture(GL_TEXTURE1); // ��ɫ����Ҫ������Ԫ����λ�ã�0�����˴�Ҳ��Ҫ���������󶨵���Ӧ������Ԫ��GL_TEXTURE0��
	unsigned int textureSpecularMap = loadTexture("skin/container2_specular.png");
	//unsigned int textureSpecular = loadTexture("skin/lighting_maps_specular_color.png");

	glActiveTexture(GL_TEXTURE2); // ��ɫ����Ҫ������Ԫ����λ�ã�0�����˴�Ҳ��Ҫ���������󶨵���Ӧ������Ԫ��GL_TEXTURE0��
	unsigned int textureEmissionMap = loadTexture("skin/matrix.jpg");

	VAO->insertTexture(0, textureDiffuseMap);
	VAO->insertTexture(1, textureSpecularMap);
	VAO->insertTexture(2, textureEmissionMap);

	// �������⣺���ö�������ָ��ʱ����һ��location���������˴����λ�õ��±���
	// �������⣺���ö�������ָ��ʱ����һ��size������ʵ�ʵĲ�һ�µ��²�����Ⱦ����ʵ�ʴ�СΪ2��ʵ����3���¶���һ��������δ��Ⱦ��
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	return VAO;
}

unsigned int RectVAOFactory::createNormalVAO()
{
	float rectVertices[] = {
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f, 1.0f, 0.0f
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);

	// Ԫ��(����)��������EBO
	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	// ��ζ������ÿ����Ҫʹ��������Ⱦ����ʱ���������Ӧ��EBO�������е��鷳��
	// ���ɶ����������Ҳ����Ԫ�ػ���������󶨡��ڰ�VAOʱ���󶨵����һ��Ԫ�ػ���������洢ΪVAO��Ԫ�ػ���������
	// Ȼ�󣬰󶨵�VAOҲ���Զ��󶨸�EBO��
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

