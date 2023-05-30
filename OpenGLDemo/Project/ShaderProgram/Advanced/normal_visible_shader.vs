#version 420 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vNormalVectorPos;
layout (location = 2) in vec2 vTexturePos;

uniform mat4 modelMat;
layout (std140, binding = 0) uniform Matrices  // 420可直接在着色器中绑定点
{
	mat4 viewMat;
	mat4 projectionMat;
};


out VsOut
{
	vec3 normal;
} vs_out;


void main()
{
	gl_Position = viewMat * modelMat * vec4(vertexPos, 1.0);

	// 法线矩阵(Normal Matrix)
	// 矩阵求逆是一项对于着色器开销很大的运算，因为它必须在场景中的每一个顶点上进行，所以应该尽可能地避免在着色器中进行求逆运算。
	// 以学习为目的的话这样做还好，但是对于一个高效的应用来说，你最好先在CPU上计算出法线矩阵，再通过uniform把它传递给着色器（就像模型矩阵一样）。
	mat3 normalMatrix = mat3(transpose(inverse(viewMat * modelMat)));
	vs_out.normal = normalize(vec3(vec4(normalMatrix * vNormalVectorPos, 0.0))); // 转换为世界坐标，法线矩阵与法向量相乘，修复不等比缩放，造成法向量不垂直，光照出错
}