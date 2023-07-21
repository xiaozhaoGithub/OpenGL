#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

uniform mat4 modelMat;
layout (std140, binding = 0) uniform Matrices  // 420可直接在着色器中绑定点
{
	mat4 viewMat;
	mat4 projectionMat;
};

uniform vec3 viewPos;
uniform vec3 lightPos;

out VsOut {
	vec3 fragPos;
	vec2 texCoords;
	vec3 tangentLightPos;
    vec3 tangentViewPos;
    vec3 tangentFragPos;
} vsOut;

void main()
{
	vsOut.fragPos = vec3(modelMat * vec4(aPos, 1.0)); // 转换为世界坐标，强制转成vec3，确保失去位移属性，以与vec3的光方向向量进行相减
	vsOut.texCoords = aTexCoords;

	// 法线矩阵，不等比缩放后法线不会变化
	mat3 normalMat = transpose(inverse(mat3(modelMat)));
	
	// TBN向量
	vec3 T = normalize(normalMat * aTangent);
	vec3 N = normalize(normalMat * aNormal);
	// 当在更大的网格上计算切线向量的时候，它们往往有很大数量的共享顶点，当法向贴图应用到这些表面时将切线向量平均化通常能获得更好更平滑的结果。
	// 这样做有个问题，就是TBN向量可能会不能互相垂直，这意味着TBN矩阵不再是正交矩阵
	// 斯密特正交化（TBN向量重正交化）
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	
	// 正交矩阵特性1：转置矩阵等于逆矩阵，直接求逆矩阵开销大
	mat3 inverseTBN = transpose(mat3(T, B, N));
	
	// 通过TBN逆矩阵将其他向量变换到切线空间
	vsOut.tangentLightPos = inverseTBN * lightPos;
	vsOut.tangentViewPos = inverseTBN * viewPos;
	vsOut.tangentFragPos = inverseTBN * vsOut.fragPos;
	
	gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
}