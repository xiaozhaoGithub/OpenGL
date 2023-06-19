#version 330 core

in VsOut {
	vec2 texturePos;
	vec3 fragPos;
	vec3 normal;
} vsIn;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform sampler2D sampler1;
uniform bool isBlinn;

void main()
{	
	vec3 texColor = texture(sampler1, vsIn.texturePos).rgb;
	vec3 ambientColor = 0.05 * texColor;
	
	vec3 lightDir = normalize(lightPos - vsIn.fragPos);
	vec3 normal = normalize(vsIn.normal);
	float diffuseFactor = max(dot(lightDir, normal), 0.0);
	vec3 diffuseColor = texColor * diffuseFactor;
	
	vec3 viewDir = normalize(viewPos - vsIn.fragPos);
	
	float specularFactor = 0.0;
	if(isBlinn) {
		// 半程向量
		vec3 halfwayDir = normalize(viewDir + lightDir);
		// 法向量半程向量余弦值
		specularFactor = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	}
	else {
		// 冯氏光照模型
		// 观察方向和反射方向余弦值
		vec3 reflectDir = reflect(-lightDir, normal);
		specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
	}
	
	vec3 specularColor = vec3(0.3) * specularFactor;
	
	FragColor = vec4(ambientColor + diffuseColor + specularColor, 1.0);
}	


	

