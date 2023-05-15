#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 position;
out vec3 normal;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;

void main()
{
	position = vec3(modelMat * vec4(aPos, 1.0)); // 世界坐标
	gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
	normal = mat3(transpose(inverse(modelMat))) * aNormal; // 法线矩阵（逆矩阵的转置）变换法线向量
}