#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 modelMat;
layout (std140, binding = 0) uniform Matrices  // 420可直接在着色器中绑定点
{
	mat4 viewMat;
	mat4 projectionMat;
};

out VsOut {
	vec2 texturePos;
	vec3 fragPos;
	vec3 normal;
} vsOut;


void main()
{
	gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
	vsOut.texturePos = vec2(aTexCoord.x, aTexCoord.y);
	vsOut.normal = aNormal;
	vsOut.fragPos = vec3(modelMat * vec4(aPos, 1.0)); // 转换为世界坐标，强制转成vec3，确保失去位移属性，以与vec3的光方向向量进行相减
}