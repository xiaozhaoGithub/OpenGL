#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 modelMat;
layout (std140, binding = 0) uniform Matrices  // 420可直接在着色器中绑定点
{
	mat4 viewMat;
	mat4 projectionMat;
};
uniform bool reverseNormals;

out VsOut {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} vsOut;


void main()
{
	// 注：此处转换观察空间坐标
	vsOut.fragPos = vec3(viewMat * modelMat * vec4(aPos, 1.0)); 
	
	// A slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
	mat3 normalMat = transpose(inverse(mat3(modelMat)));
	vsOut.normal = normalMat * (reverseNormals ? -aNormal : aNormal);
	
	vsOut.texCoords = aTexCoords;
	
	gl_Position = projectionMat * viewMat * modelMat * vec4(aPos, 1.0);
}