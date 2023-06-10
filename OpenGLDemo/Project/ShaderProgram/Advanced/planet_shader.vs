#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;


uniform mat4 modelMat;
layout (std140, binding = 0) uniform Matrices  // 420可直接在着色器中绑定点
{
	mat4 viewMat;
	mat4 projectionMat;
};
// uniform mat4 normalMat; // 法线矩阵

out vec2 TexCoords;

void main()
{
	TexCoords = aTexCoords;
    gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0f); 
}
