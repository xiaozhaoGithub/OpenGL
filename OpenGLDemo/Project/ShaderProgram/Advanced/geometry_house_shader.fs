#version 330 core

in vec3 fragColor;
out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

void main()
{	
	FragColor = vec4(fragColor, 1.0);
}	


	

