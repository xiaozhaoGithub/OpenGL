#ifndef TEXTUREHELPER_H_
#define TEXTUREHELPER_H_

#include <string>
#include <vector>

#include "glad/glad.h"
#include "glm.hpp"
#include "Shader.h"

namespace TextureHelper 
{
	struct TexParam {
		int wrapS = GL_REPEAT;
		int wrapT = GL_REPEAT;
		int internalFormat3 = GL_RGB;
		int internalFormat4 = GL_RGBA;
	};

	// utility function for loading a 2D texture from file
	unsigned int loadTexture(char const* path, const TexParam& param = TexParam());
	unsigned int loadCubemap(const std::vector<std::string>& faces, const TexParam& param = TexParam());
}

#endif

