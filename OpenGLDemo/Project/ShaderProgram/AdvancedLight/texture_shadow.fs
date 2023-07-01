#version 330 core

in VsOut {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
	vec4 lightSpaceFragPos;
} vsIn;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform vec3 viewPos;
uniform vec3 lightPos;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform bool isBlinn;
uniform bool isGammaCorrection;

float calcShadow(vec4 lightSpaceFragPos)
{
	// 1. gl_Postion内置变量，由顶点着色器输出时，自动透视除法，而lightSpaceFragPos不会
	vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
	
	// 2. 透视除法后，为标准设备化坐标（NDC: 范围-1，1），需变化到深度值范围（0，1）进行比较
	projCoords = projCoords * 0.5 + 0.5;
	
	// 3. 获取最近的深度值（深度贴图保存的深度值）
	float closestDepth = texture(shadowMap, projCoords.xy).r; // 深度为单通道，取r (rgba)
	
	// 4. 当前深度值
	float curDepth = projCoords.z;
	
	float shadow = curDepth > closestDepth ? 1.0 : 0.0;
	
	return shadow;
}

void main()
{	
	vec3 texColor = texture(diffuseTexture, vsIn.texCoords).rgb;

	vec3 lightColor = vec3(0.3);

	vec3 ambientColor = 0.3 * lightColor;
	
	// diffuse
	vec3 lightDir = normalize(lightPos - vsIn.fragPos);
	vec3 normal = normalize(vsIn.normal);
	float diffuseFactor = max(dot(lightDir, normal), 0.0);
	vec3 diffuseColor = diffuseFactor * lightColor;
	
	// specular
	vec3 viewDir = normalize(viewPos - vsIn.fragPos);
	float specularFactor = 0.0;
	if(isBlinn) {
		// 半程向量
		vec3 halfwayDir = normalize(lightDir + viewDir);
		// 法向量半程向量余弦值
		specularFactor = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	}
	else {
		// 冯氏光照模型
		// 观察方向和反射方向余弦值
		vec3 reflectDir = reflect(-lightDir, normal);
		specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
	}
	
	vec3 specularColor = specularFactor * lightColor;
	
	// attenuation 
	// float maxDistance = 1.5;
	float distance = length(lightPos - vsIn.fragPos);
	
	// 不同情况。不同的衰减效果更好
	// 真实的物理世界：float attenuation = 1.0 / (distance * distance);
    // float attenuation = 1.0 / (isGammaCorrection ? distance * distance : distance);
	
	// 衰减导致光线变暗，若设置，则需调整光亮
	// ambientColor *= attenuation;
	// diffuseColor *= attenuation;
	// specularColor *= attenuation;
	
	// shadow 阴影：1.0 
	float shadow = calcShadow(vsIn.lightSpaceFragPos);
	vec3 lighting = (ambientColor + (1.0 - shadow) * (diffuseColor + specularColor)) * texColor;
	
	FragColor = vec4(lighting, 1.0);
	
	// if(isGammaCorrection) {
		// float gamma = 2.2;
		// FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma)); // 大多数设备屏幕亮度会以2.2次幂输出，降低亮度
	// }
}	


	

