#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VsOut {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} fsIn;

struct Light {
	vec3 position;
	vec3 color;
};

uniform vec3 lightColor;

void main()
{	
	vec3 result = lightColor;
	// 超过阈值，则输出泛光阈值颜色
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
	BrightColor = brightness > 1.0 ? vec4(result, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
	
	FragColor = vec4(result, 1.0);
}	


	

