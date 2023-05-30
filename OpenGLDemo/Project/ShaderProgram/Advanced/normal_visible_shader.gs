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
	vec3 normal;
} gs_in[];


const float magnitude = 0.01;
void GenerateLine(int index)
{
	
	gl_Position = projectionMat * (gl_in[index].gl_Position);
	EmitVertex();	
	
	gl_Position = projectionMat * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * magnitude);
	EmitVertex();	

	EndPrimitive();	
}

void main()
{
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}