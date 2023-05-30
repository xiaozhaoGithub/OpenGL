#version 420 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out; 
layout (std140, binding = 0) uniform Matrices  // 420可直接在着色器中绑定点
{
	mat4 viewMat;
	mat4 projectionMat;
};
// 内建变量可能如下
// in gl_Vertex
// {
    // vec4  gl_Position;
    // float gl_PointSize;
    // float gl_ClipDistance[];
// } gl_in[];

uniform float time;

in VsOut
{
	vec2 texCoords;
	vec3 normalVector;
	vec3 FragPos;
} gs_in[];


// 注：若存在几何着色器，则顶点着色器的输出，需要在几何着色器中进行传递。
out vec2 texturePos;
out vec3 normalVector;
out vec3 FragPos;

void GenerateLine(int index)
{
	float magnitude = 1.0;
	
	gl_Position = projectionMat * (gl_in[index].gl_Position);
	EmitVertex();	
	
	gl_Position = projectionMat * (gl_in[index].gl_Position + vec4(normalize(gs_in[index].normalVector), 0.0) * magnitude);
	EmitVertex();	

	EmitVertex();	
}

void main()
{
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}