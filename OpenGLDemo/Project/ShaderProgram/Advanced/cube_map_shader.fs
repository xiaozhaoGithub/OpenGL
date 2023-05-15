#version 330 core

in vec3 texturePos; // 使用立方体的方向向量采样纹理

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform samplerCube sampler1; // 立方体贴图的纹理采样器

void main()
{	
	vec4 texColor = texture(sampler1, texturePos);
	FragColor = texColor;
}	


	

