#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 modelMat;
layout (std140, binding = 0) uniform Matrices  // 420可直接在着色器中绑定点
{
	mat4 viewMat;
	mat4 projectionMat;
};

out vec3 position;
out vec3 normal;

void main()
{
	position = vec3(modelMat * vec4(aPos, 1.0)); // 世界坐标
	gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
	normal = mat3(transpose(inverse(modelMat))) * aNormal; // 法线矩阵（逆矩阵的转置）变换法线向量
}