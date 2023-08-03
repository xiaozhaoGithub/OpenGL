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

uniform sampler2D diffuseTexture;
uniform Light lights[4];

void main()
{	
	vec3 texColor = texture(diffuseTexture, fsIn.texCoords).rgb;
	vec3 normal = normalize(fsIn.normal);

	// ambient
	vec3 ambientColor = 0.0 * texColor;
	
	// lighting
	vec3 diffuseColors = vec3(0.0);
	for(int i = 0; i < 4; i++) {
		vec3 lightDir = normalize(lights[i].position - fsIn.fragPos);
		float diffuseFactor = max(dot(lightDir, normal), 0.0);
		vec3 diffuseColor = lights[i].color * diffuseFactor * texColor;
		
		// attenuation
		float distance = length(lights[i].position - fsIn.fragPos);
		diffuseColor *= 1.0 / (distance * distance); // 真实的物理世界为二次函数衰减，配合gamma校正使用
		
		diffuseColors += diffuseColor;
	}
	
	vec3 result = ambientColor + diffuseColors;
	
	// 超过阈值，则输出泛光阈值颜色
	float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
	BrightColor = brightness > 1.0 ? vec4(result, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
	
	FragColor = vec4(result, 1.0);
}	


	

