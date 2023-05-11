#version 330 core

in vec2 texturePos;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform sampler2D sampler1;

void main()
{	
	vec4 texColor = texture(sampler1, texturePos);
	
	
	// 反相
	FragColor = vec4(vec3(1.0 - texColor), 1.0);
	
	// 灰度
	float average = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
    FragColor = vec4(average, average, average, 1.0);
}	


	

