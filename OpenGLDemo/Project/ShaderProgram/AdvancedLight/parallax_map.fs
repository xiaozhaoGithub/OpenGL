#version 330 core
// 视差贴图是提升场景细节非常好的技术，但是使用的时候还是要考虑到它会带来一点不自然。
// 大多数时候视差贴图用在地面和墙壁表面，这种情况下查明表面的轮廓并不容易，同时观察角度往往趋向于垂直于表面

in VsOut {
	vec3 fragPos;
	vec2 texCoords;
	vec3 tangentLightPos;
    vec3 tangentViewPos;
    vec3 tangentFragPos;
} fsIn;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float heightScale;

// 通过深度贴图的高度对观察向量缩放，返回位移后的纹理坐标（模拟点位移效果）
vec2 parallaxMapping(vec2 texCoords, vec3 viewDir)
{
	float height = texture(depthMap, texCoords).r;
	return texCoords - viewDir.xy * (height * heightScale); // 纹理坐标 - 向量P
}

// 陡峭视差贴图（多个样本获取向量P到B）
// 求第一个层深度 > 深度贴图值的坐标（此点纹理坐标位于表面下方）
vec2 steepParallaxMapping(vec2 texCoords, vec3 viewDir, bool isAntialiasing)
{
	// 算法提升：垂直看表面纹理时，使用更少样本，角度变大时，增加样本
	const float minNumLayers = 8;
	const float maxNumLayers = 32;
	
	// mix(a, b, 0.7) == a * (1 - 0.3) + b * 0.7， 90度时 = 8 * (1 - 0)
	// 正z向量相当于切线空间的法线
	float layerNum = mix(minNumLayers, maxNumLayers, abs(dot(vec3(0, 0, 1), viewDir)));
	float layerDepth = 1.0 / layerNum;
	
	float curLayerDepth = 0.0;
	
	vec2 p = viewDir.xy / viewDir.z * heightScale;
	vec2 deltaTexCoords = p / layerNum;
	
	vec2  curTexCoords = texCoords;
	float curDepthMapValue = texture(depthMap, curTexCoords).r;
	
	while(curLayerDepth < curDepthMapValue) {
		curTexCoords -= deltaTexCoords;
		curDepthMapValue = texture(depthMap, curTexCoords).r;

		curLayerDepth += layerDepth;
	}
	
	vec2 finalTexCoords = curTexCoords;
	
	if(isAntialiasing) {
		// get texture coordinates before collision (reverse operations)
		vec2 prevTexCoords = curTexCoords + deltaTexCoords;

		// get depth after and before collision for linear interpolation
		float afterDepth  = curDepthMapValue - curLayerDepth;
		float beforeDepth = texture(depthMap, prevTexCoords).r - curLayerDepth + layerDepth;
	 
		// interpolation of texture coordinates
		float weight = afterDepth / (afterDepth - beforeDepth);
		finalTexCoords = prevTexCoords * weight + curTexCoords * (1.0 - weight);
	}
	
	return finalTexCoords;
}

void main()
{	
	vec3 viewDir = normalize(fsIn.tangentViewPos - fsIn.tangentFragPos);
	
	// 1 普通视差贴图
	// vec2 texCoords = parallaxMapping(fsIn.texCoords, viewDir);
	// 2 陡峭视差贴图
	// vec2 texCoords = steepParallaxMapping(fsIn.texCoords, viewDir, false);
	
	// 3 视差遮蔽贴图（锯齿可通过增加样本的方式减少这个问题，也可以采用此方法，浮雕视差贴图性能消耗大）
	vec2 texCoords = steepParallaxMapping(fsIn.texCoords, viewDir, true);

	// 丢弃超出纹理坐标的片段
	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
		discard;
	
	// obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, texCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
	
	// diffuse color
    vec3 color = texture(diffuseMap, texCoords).rgb;
	
    // ambient
    vec3 ambient = 0.1 * color;
	
    // diffuse
    vec3 lightDir = normalize(fsIn.tangentLightPos - fsIn.tangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
	
    // specular
    // vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;
	
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}	


	

