#ifndef CHECKERROR_H
#define CHECKERROR_H

#include <string>
#include <iostream>

#include "glad/glad.h"

#define glCheckError() CheckError::glCheckErrorHandle(__FILE__, __LINE__) 

namespace CheckError
{
	// ����ϴδ��󣬶���������ʱ����ʾ��һ������glGetError���ú󣬻������һ�εĴ���
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

