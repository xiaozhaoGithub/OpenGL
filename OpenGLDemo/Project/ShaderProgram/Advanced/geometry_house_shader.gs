#version 420 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out; // N - 2个三角形

// 内建变量可能如下
// in gl_Vertex
// {
    // vec4  gl_Position;
    // float gl_PointSize;
    // float gl_ClipDistance[];
// } gl_in[];

in VsOut
{
	vec3 color;
} gs_in[]; // 几何着色器，通常是以一组数据，即使只有一个点

out vec3 fragColor;

void buildHouses(vec4 position)
{
	// 当发射一个顶点的时候，每个顶点将会使用最后在fColor中储存的值，来用于片段着色器的运行
	fragColor = gs_in[0].color;
	
	// 显示效果与发射顶点数据次序相关
	gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);
	EmitVertex();	
	
	gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0);
	EmitVertex();
	
	gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0);
	EmitVertex();
	
	gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0);
	EmitVertex();	
	
	fragColor = vec3(1.0, 1.0, 1.0);
	gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0);
	EmitVertex();
	
	EndPrimitive();
}

void main()
{
	buildHouses(gl_in[0].gl_Position);
}