#include "TextureHelper.h"

#include <iostream>

#include "stb_image.h"	
#include "CheckError.h"

unsigned int TextureHelper::loadTexture(const TexParam& param)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int internalformat = param.internalFormat3;

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, param.width, param.height, 0, param.format, GL_FLOAT, param.data);
	glGenerateMipmap(GL_TEXTURE_2D); // ����������֮�����glGenerateMipmap�����Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);

	return textureID;
}

unsigned int TextureHelper::loadTexture(char const* path, const TexParam& param)
{
	int width, height, nrComponents;
	void* data = nullptr;

	stbi_set_flip_vertically_on_load(true);

	switch (param.type) {
	case GL_UNSIGNED_BYTE:
		data = stbi_load(path, &width, &height, &nrComponents, 0);
	break;
	case GL_FLOAT:
		data = stbi_loadf(path, &width, &height, &nrComponents, 0);
		break;
	default:
		break;
	}

	unsigned int textureId = 0;
	if (data) {
		glGenTextures(1, &textureId);

		GLenum internalformat;
		GLenum format;
		if (nrComponents == 1) {
			internalformat = GL_RED;
			format = GL_RED;
		}
		else if (nrComponents == 3) {
			internalformat = param.internalFormat3;
			format = GL_RGB;
		}
		else if (nrComponents == 4) {
			internalformat = param.internalFormat4;
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, param.type, data);
		glGenerateMipmap(GL_TEXTURE_2D); // ����������֮�����glGenerateMipmap�����Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param.wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param.wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

		glCheckError();
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}

	return textureId;
}

unsigned int TextureHelper::loadCubemap(const std::vector<std::string>& faces, const TexParam& param)
{
	unsigned int textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	for (unsigned int i = 0; i < faces.size(); i++) {
		int width, height, nrComponents;
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data) {
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
		else {
			std::cout << "Texture failed to load at path: " << faces[i].c_str() << std::endl;
		}
	}

	if (faces.size() == 0) {
		for (unsigned int i = 0; i < 6; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, param.internalFormat3, param.width, param.height, 0, param.format, param.type, nullptr);
		}
	}

	// �����ô���������֮�������������ܲ��ܻ���һ���棨����һЩӲ�����ƣ����߽����͸��������ʹ�ñ߽�ֵ���ƽ��
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // ����z��������
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureId;
}