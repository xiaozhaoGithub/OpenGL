#version 330 core
out vec4 FragColor;
out vec4 vertexPos;
in vec4 vertexColor;
void main() 
{
	FragColor = vertexColor;
}