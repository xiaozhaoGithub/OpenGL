#version 420 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out; // N - 2个三角形

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

vec3 getNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	
	return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 2.0;
	vec3 direction = normal * ((sin(time) + 1.0 ) / 2.0) * magnitude; // 外爆炸 0.0 - 1.0 * magnitude
	
	return position + vec4(direction, 0.0);
}

void main()
{
	vec3 normal = getNormal();
	
	gl_Position = explode(gl_in[0].gl_Position, normal);
	texturePos = gs_in[0].texCoords;
	normalVector = gs_in[0].normalVector;
	FragPos = gs_in[0].FragPos;
	EmitVertex();	

	gl_Position = explode(gl_in[1].gl_Position, normal);
	texturePos = gs_in[1].texCoords;
	normalVector = gs_in[1].normalVector;
	FragPos = gs_in[1].FragPos;
	EmitVertex();	
	
	gl_Position = explode(gl_in[2].gl_Position, normal);
	texturePos = gs_in[2].texCoords;
	normalVector = gs_in[2].normalVector;
	FragPos = gs_in[2].FragPos;
	EmitVertex();	
	
	EndPrimitive();
}