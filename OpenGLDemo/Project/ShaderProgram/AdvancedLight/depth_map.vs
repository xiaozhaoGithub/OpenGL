#version 420 core
layout (location = 0) in vec3 aPos;

uniform mat4 modelMat;
uniform mat4 lightViewMat;
uniform mat4 lightProjMat;

void main()
{
	gl_Position = lightProjMat * lightViewMat * modelMat * vec4(aPos, 1.0);
}