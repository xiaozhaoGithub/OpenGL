#version 420 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowTransformMats[6];

out vec4 fragPos;

void main()
{
	for(int face = 0; face < 6; face++) {
		gl_Layer = face; // 内置变量，指定渲染cube map的哪一面
		for(int i = 0; i < 3; i++) {
			fragPos = gl_in[i].gl_Position;
			gl_Position = shadowTransformMats[face] * fragPos;
			EmitVertex();
		}
		
		EndPrimitive();
	}
}