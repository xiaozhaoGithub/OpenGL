#version 330 core

in VsOut {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} fsIn;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform float farPlane;

uniform sampler2D diffuseTexture;
uniform samplerCube shadowCubeMap;

uniform bool isBlinn;
uniform bool isShowShadow;

float calcShadow(vec3 fragPos, float bias)
{
	// 通过方向向量采样立方体贴图深度值
	vec3 fragToLight = fragPos - lightPos;
	
	float closestDepth = texture(shadowCubeMap, fragToLight).r;
	closestDepth *= farPlane;
	
	float curDepth = length(fragToLight);

	float shadow;
	if(curDepth / farPlane > 1.0) {
		shadow = 0.0; // 当超出光锥范围，投影坐标z > 0，即当前深度值大于0，这个不设为阴影
	}
	else {
		shadow = curDepth - bias > closestDepth ? 1.0 : 0.0;
	}
	
	// FragColor = vec4(vec3(closestDepth / farPlane), 1.0);   
	return shadow;
}

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float calcPcfShadow(vec3 fragPos, float bias)
{
	// 通过方向向量采样立方体贴图深度值
	vec3 fragToLight = fragPos - lightPos;
	float curDepth = length(fragToLight);
	
	float viewDistance = length(viewPos - fragPos);

	// float diskRadius = 0.05;
	float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
	
	float shadow = 0.0;
	
	int samples = 20;
	for(int i = 0; i < samples; ++i) {
		float closestDepth = texture(shadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= farPlane;   // Undo mapping [0;1]
		if(curDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);

	return shadow;
}

void main()
{	
	vec3 texColor = texture(diffuseTexture, fsIn.texCoords).rgb;

	vec3 lightColor = vec3(0.3);

	vec3 ambientColor = 0.3 * lightColor;
	
	// diffuse
	vec3 lightDir = normalize(lightPos - fsIn.fragPos);
	
	vec3 normal = normalize(fsIn.normal);
	float diffuseFactor = max(dot(lightDir, normal), 0.0);
	vec3 diffuseColor = diffuseFactor * lightColor;
	
	// specular
	vec3 viewDir = normalize(viewPos - fsIn.fragPos);
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
	
	// shadow 阴影：1.0 
	// 解决阴影失真
	// 方法1
	// float bias = 0.08; // 偏移过大可能会导致阴影失真（悬浮）
	float bias = 0.15;
	// 方法2(效果更好，依赖光源与法线)
	// 像地板这样的表面几乎与光源垂直，得到的偏移就很小，而比如立方体的侧面这种表面得到的偏移就更大
	// float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); // 越接近垂直，dot越大，偏移越小
	
	// float shadow = isShowShadow ? calcShadow(fsIn.fragPos, bias) : 0.0;
	float shadow = isShowShadow ? calcPcfShadow(fsIn.fragPos, bias) : 0.0;
	vec3 lighting = (ambientColor + (1.0 - shadow) * (diffuseColor + specularColor)) * texColor;
	
	FragColor = vec4(lighting, 1.0);
}	


	

