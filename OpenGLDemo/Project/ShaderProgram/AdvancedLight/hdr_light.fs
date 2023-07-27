#version 330 core

in VsOut {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} fsIn;

struct Light {
	vec3 position;
	vec3 color;
};

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform sampler2D diffuseTexture;
uniform Light lights[16];

void main()
{	
	vec3 texColor = texture(diffuseTexture, fsIn.texCoords).rgb;
	vec3 normal = normalize(fsIn.normal);

	// ambient
	vec3 ambientColor = 0.0 * texColor;
	
	// lighting
	vec3 diffuseColors = vec3(0.0);
	for(int i = 0; i < 16; i++) {
		vec3 lightDir = normalize(lights[i].position - fsIn.fragPos);
		float diffuseFactor = max(dot(normal, lightDir), 0.0);
		vec3 diffuseColor = lights[i].color * diffuseFactor * texColor;
		
		// attenuation
		float distance = length(lights[i].position - fsIn.fragPos);
		diffuseColor *= 1.0 / (distance * distance); // 真实的物理世界为二次函数衰减，配合gamma校正使用
		
		diffuseColors += diffuseColor;
	}
	
	FragColor = vec4(ambientColor + diffuseColors, 1.0);
}	


	

