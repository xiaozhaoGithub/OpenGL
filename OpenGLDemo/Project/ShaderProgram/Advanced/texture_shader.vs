#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 modelMat;
layout (std140, binding = 0) uniform Matrices  // 420可直接在着色器中绑定点
{
	mat4 viewMat;
	mat4 projectionMat;
};

out vec2 texturePos;

void main()
{
	gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
	texturePos = vec2(aTexCoord.x, aTexCoord.y);
}