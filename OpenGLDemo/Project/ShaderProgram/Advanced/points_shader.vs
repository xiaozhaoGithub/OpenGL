#version 420 core
layout (location = 0) in vec3 aPos;

uniform mat4 modelMat;
layout (std140, binding = 0) uniform Matrices  // 420可直接在着色器中绑定点
{
	mat4 viewMat;
	mat4 projectionMat;
};

// 整型变量gl_VertexID储存了正在绘制顶点的当前ID。
// 当（使用glDrawElements）进行索引渲染的时候，这个变量会存储正在绘制顶点的当前索引。
// 当（使用glDrawArrays）不使用索引进行绘制的时候，这个变量会储存从渲染调用开始的已处理顶点数量。
void main()
{
	gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
	gl_PointSize = gl_Position.z * 5;
}