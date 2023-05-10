#version 330 core

in vec2 texturePos;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform sampler2D sampler1;

void main()
{	
	vec4 texColor = texture(sampler1, texturePos);
	FragColor = texColor;
}	


	

