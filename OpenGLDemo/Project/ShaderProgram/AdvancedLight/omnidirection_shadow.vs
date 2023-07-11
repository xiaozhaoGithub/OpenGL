#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 modelMat;
layout (std140, binding = 0) uniform Matrices  // 420可直接在着色器中绑定点
{
	mat4 viewMat;
	mat4 projectionMat;
};
uniform bool reverseNormals;

out VsOut {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} vsOut;


void main()
{
	vsOut.fragPos = vec3(modelMat * vec4(aPos, 1.0)); // 转换为世界坐标，强制转成vec3，确保失去位移属性，以与vec3的光方向向量进行相减
	
	// 法线矩阵，不等比缩放后法线不会变化
	if(reverseNormals) // A slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
        vsOut.normal = transpose(inverse(mat3(modelMat))) * (-1.0 * aNormal);
    else
        vsOut.normal = transpose(inverse(mat3(modelMat))) * aNormal;
	
	vsOut.texCoords = aTexCoords;
	
	gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
}