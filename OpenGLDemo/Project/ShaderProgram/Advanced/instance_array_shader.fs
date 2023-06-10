#version 330 core

in VsOut
{
	vec3 color;
} fs_in;


out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

void main()
{	
	FragColor = vec4(fs_in.color, 1.0);
}	


	

