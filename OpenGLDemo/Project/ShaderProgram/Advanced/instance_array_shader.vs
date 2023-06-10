#version 420 core
layout (location = 0) in vec2 vertexPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vOffset;

uniform mat4 modelMat;
layout (std140, binding = 0) uniform Matrices  // 420可直接在着色器中绑定点
{
	mat4 viewMat;
	mat4 projectionMat;
};

out VsOut
{
	vec3 color;
} vs_out;

void main()
{
	vec3 pos = vec3(vertexPos * (gl_InstanceID / 100.0), 2.0);
	gl_Position = projectionMat * viewMat * modelMat * (vec4(pos, 1.0) + vec4(vOffset, 0.0, 0.0));
	vs_out.color = vColor;
}