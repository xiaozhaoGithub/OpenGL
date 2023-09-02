#ifndef CHECKERROR_H
#define CHECKERROR_H

#include <string>
#include <iostream>

#include "glad/glad.h"

#define glCheckError() CheckError::glCheckErrorHandle(__FILE__, __LINE__) 

namespace CheckError
{
	// 检测上次错误，多个错误存在时，提示第一个错误，glGetError调用后，会清空上一次的错误
	GLenum glCheckErrorHandle(const char *file, int line);

	void APIENTRY glDebugOutput(GLenum source,
		GLenum type,
		unsigned int id,
		GLenum severity,
		GLsizei length,
		const char *message,
		const void *userParam);
}
#endif

