#version 330 core

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

void main()
{	
	vec3 viewDir = normalize(fsIn.tangentViewPos - fsIn.tangentFragPos);
	vec2 texCoords = parallaxMapping(fsIn.texCoords, viewDir);
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


	

