#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 outColor;
out vec4 vertexColor;
uniform float xOffset;
void main()
{
	gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);
	vertexColor = vec4(outColor, 1.0);
}