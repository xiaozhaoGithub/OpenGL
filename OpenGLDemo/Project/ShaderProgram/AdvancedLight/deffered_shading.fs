#version 330 core

out vec4 FragColor;
in vec2 texturePos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light{
    vec3 position;
    vec3 color;
    float linear;
    float quadratic;
    float radius;
};

const int NR_LIGHTS=32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{
    vec3 fragPos = texture(gPosition,texturePos).rgb;
    vec3 normal = texture(gNormal,texturePos).rgb;
    vec3 diffuse = texture(gAlbedoSpec,texturePos).rgb;
    float specular = texture(gAlbedoSpec,texturePos).a;
    
    // then calculate lighting as usual
    vec3 lighting = diffuse * 0.1;// hard-coded ambient component
    vec3 viewDir = normalize(viewPos - fragPos);
    for(int i = 0; i < NR_LIGHTS; ++i) {
        float distance = length(lights[i].position-fragPos);
        // 由于GLSL并行，此判断实际无用
        if(distance < lights[i].radius) {
            // diffuse
            vec3 lightDir = normalize(lights[i].position - fragPos);
            vec3 diffuseColor = max(dot(normal,lightDir),0.) * diffuse * lights[i].color;
            
            // specular
            vec3 halfwayDir = normalize(lightDir+viewDir);
            float spec = pow(max(dot(normal, halfwayDir), 0.), 16.);
            vec3 specularColor = lights[i].color * spec * specular;
            
            // attenuation
            float attenuation=1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * distance * distance);// 衰减方程
            diffuseColor *= attenuation;
            specularColor *= attenuation;
            
            lighting += diffuseColor + specularColor;
        }
    }
    
    FragColor=vec4(lighting, 1.0);
}

