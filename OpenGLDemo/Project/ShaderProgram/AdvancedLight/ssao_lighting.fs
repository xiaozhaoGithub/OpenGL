#version 330 core

out vec4 FragColor;
in vec2 texturePos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gSsao;

struct Light{
    vec3 position;
    vec3 color;
    float linear;
    float quadratic;
    float radius;
};

uniform Light light;

void main()
{
    // 注: ssao光照计算位于观察空间中
    vec3 fragPos = texture(gPosition, texturePos).rgb;
    vec3 normal = texture(gNormal, texturePos).rgb;
    vec3 diffuse = texture(gAlbedo, texturePos).rgb;
    float ambientOcclusion = texture(gSsao, texturePos).r;
    
    // then calculate lighting as usual
    vec3 ambientColor = vec3(0.3 * ambientOcclusion); // 注: 环境光乘上遮蔽因子
    vec3 lighting = ambientColor;
    vec3 viewPos = vec3(0, 0, 0);
    vec3 viewDir = normalize(viewPos - fragPos); 
    
    // diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 diffuseColor = max(dot(normal, lightDir), 0.0) * diffuse * light.color;
    
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0);
    vec3 specularColor = light.color * spec;
    
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);// 衰减方程
    diffuseColor *= attenuation;
    specularColor *= attenuation;
    
    lighting += diffuseColor + specularColor;
    
    FragColor= vec4(lighting, 1.0);
}

