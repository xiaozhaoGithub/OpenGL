#version 330 core

in vec3 position;
in vec3 normal;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform samplerCube sampler1; // 立方体贴图的纹理采样器
uniform vec3 cameraPos;

void main()
{	
	float ratio = 1.00 / 1.52; // 介质：空气到玻璃
	vec3 I = normalize(position - cameraPos);
	vec3 R = refract(I, normalize(normal), ratio);
	FragColor = vec4(texture(sampler1, R).rgb, 1.0);
}	


	

