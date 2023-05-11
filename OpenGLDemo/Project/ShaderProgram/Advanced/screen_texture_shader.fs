#version 330 core

in vec2 texturePos;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform sampler2D sampler1;


const float offset = 1.0 / 300.0;

// 锐化核（麻醉效果）
float sharpenKernel[9] = float[](
	-1, -1, -1,
	-1,  9, -1,
	-1, -1, -1
);


// 模糊核
float blurKernel[9] = float[](
	1.0 / 16, 2.0 / 16, 1.0 / 16,
	2.0 / 16, 4.0 / 16, 2.0 / 16,
	1.0 / 16, 2.0 / 16, 1.0 / 16  
);

// 边缘检测核（核高亮了所有的边缘，而暗化了其它部分，在我们只关心图像的边角的时候是非常有用的）
float edgeDetectionKernel[9] = float[](
	1, 1, 1,
	1, -8, 1,
	1, 1, 1
);
	
// 数组作为函数形参
vec4 postProcess(float kernel[9])
{
	// GLSL严格遵守数组定义，与C语言中不同
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset), 
		vec2(0.0, offset), 
		vec2(offset, offset),
		
        vec2(-offset, 0.0), 
		vec2(0.0, 0.0), 
		vec2(offset, 0.0),
		
        vec2(-offset, -offset), 
		vec2(0.0, -offset), 
		vec2(offset, -offset)
	);
		
	
	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++) {
		sampleTex[i] = vec3(texture(sampler1, texturePos.st + offsets[i]));
	}
	
	vec3 color = vec3(0.0);
	for(int i = 0; i < 9; i++) {
		color += sampleTex[i] * kernel[i];
	}
	
	return vec4(color, 1.0);
}


void main()
{	
	vec4 texColor = texture(sampler1, texturePos);
	
	// 反相
	FragColor = vec4(vec3(1.0 - texColor), 1.0);
	
	// 灰度化
	float average = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
    FragColor = vec4(average, average, average, 1.0);
	
	// 锐化
	FragColor = postProcess(sharpenKernel);
	
	// 模糊
	FragColor = postProcess(blurKernel);
	
	// 边缘检测
	FragColor = postProcess(edgeDetectionKernel);
}	


	

