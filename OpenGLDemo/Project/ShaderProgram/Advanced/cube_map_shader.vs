#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texturePos;

uniform mat4 viewMat;
uniform mat4 projectionMat;

void main()
{
	texturePos = aPos;
	vec4 pos = projectionMat * viewMat * vec4(aPos, 1.0);
	gl_Position = pos.xyww; // 目的是为了使天空盒提前深度测试，z / w分量等于深度值，w / w得到最大的深度值1.0
}