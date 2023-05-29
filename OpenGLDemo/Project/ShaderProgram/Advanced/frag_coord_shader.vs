#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 modelMat;
layout (std140) uniform Matrices 
{
	mat4 viewMat;
	mat4 projectionMat;
};
// 420后，可在着色器中绑定
// layout(std140, binding = 2) uniform Matrices { ... };

out VS_OUT 
{
	vec2 texturePos;
} vs_out;

 
void main()
{
	gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
	vs_out.texturePos = aTexCoord;
}