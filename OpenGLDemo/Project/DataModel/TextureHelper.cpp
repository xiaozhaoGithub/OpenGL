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
	glGenerateMipmap(GL_TEXTURE_2D); // 在生成纹理之后调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);

	return textureID;
}

unsigned int TextureHelper::loadTexture(char const* path, const TexParam& param)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {
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

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); // 在生成纹理之后调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param.wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param.wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glCheckError();
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}

	stbi_image_free(data);

	return textureID;
}

unsigned int TextureHelper::loadCubemap(const std::vector<std::string>& faces, const TexParam& param)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

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

	// 因正好处于两个面之间的纹理坐标可能不能击中一个面（由于一些硬件限制），边界产生透明线条，使用边界值环绕解决
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // 类似z方向向量
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}