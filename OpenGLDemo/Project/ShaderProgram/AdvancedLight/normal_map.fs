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

void main()
{	
	// obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, fsIn.texCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
   
    // get diffuse color
    vec3 color = texture(diffuseMap, fsIn.texCoords).rgb;
	
    // ambient
    vec3 ambient = 0.1 * color;
	
    // diffuse
    vec3 lightDir = normalize(fsIn.tangentLightPos - fsIn.tangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
	
    // specular
    vec3 viewDir = normalize(fsIn.tangentViewPos - fsIn.tangentFragPos);
    // vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;
	
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}	


	

