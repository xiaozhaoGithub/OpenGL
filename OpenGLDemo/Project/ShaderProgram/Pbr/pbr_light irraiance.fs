#version 330 core

out vec4 FragColor;
in VsOut {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
} fsIn;

// material parameters
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos;

uniform samplerCube irradianceMap;

const float PI = 3.14159265359;

/**
 * @brief 法线分布函数
 * 从统计学上近似地表示了与某些（半程）向量h取向一致的微平面的比率。举例来说，假设给定向量h,如果我们的微平面中有35%与向量h取向一致，
 * 则法线分布函数或者说NDF将会返回0.35。
 * 
 * @param n 法向量
 * @param h 用来与平面上微平面做比较用的半程向量
 * @param roughness 表面粗糙度
 *
 * @return 返回与向量h取向一致的微平面的比率
 */
float DistributionGGX(vec3 n, vec3 h, float roughness) 
{
    float a = roughness * roughness;
    float a2 = a * a;
    float nDotH = max(dot(n, h), 0.0);
    float nDotH2 = nDotH * nDotH;
    
    float nom = a2;
    float denom = (nDotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

/**
 * @brief 几何函数
 * 从统计学上近似的求得了微平面间相互遮蔽的比率，这种相互遮蔽会损耗光线的能量。粗糙度较高的表面其微平面间相互遮蔽的概率就越高
 * 
 * @param n 法向量
 * @param roughness 表面粗糙度
 *
 * @return 遮蔽的比率
 */
 float GeometrySchlickGGX(float nDotV, float roughness)
 {
     float r = roughness + 1.0;
     float k = (r * r) / 8.0; // k是α的重映射(Remapping)，直接光照和IBL光照计算公式不同

     float nom = nDotV;
     float denom = nDotV * (1.0 - k) + k;

     return nom / denom;
 }

 float GeometrySmith(vec3 n, vec3 v, vec3 l, float roughness)
 {
     float nDotV = max(dot(n, v), 0.0);
     float nDotL = max(dot(n, l), 0.0);
     float ggx1 = GeometrySchlickGGX(nDotV, roughness);
     float ggx2 = GeometrySchlickGGX(nDotL, roughness);

    // 利用史密斯法将观察方向（几何遮蔽(Geometry Obstruction)）和光线方向向量（几何阴影(Geometry Shadowing)）都考虑进去
     return ggx1 * ggx2;
 }

/**
 * @brief 菲涅尔方程
 * 
 * @param f0 表示平面的基础反射率，它是利用所谓折射指数(Indices of Refraction)或者说IOR计算得出的
 * 0°入射角的反射率，或者说是直接(垂直)观察表面时有多少光线会被反射
 * @param cosTheta h dot n or v dor n.
 * @param roughness 粗糙度
 *
 * @return 返回被反射的光线对比光线被折射的部分所占的比率
 */
vec3 FresnelSchlick(float cosTheta, vec3 f0) 
{
    // 约束范围，避免黑点
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cosTheta, 0.0 , 1.0) , 5.0);
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}  

void main()
{
    vec3 normal = normalize(fsIn.normal);
    vec3 viewDir = normalize(camPos - fsIn.fragPos);

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, metallic);// metallic workflow

     // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) {
        // calculate per-light radiance
        vec3 lightDir = normalize(lightPositions[i] - fsIn.fragPos);
        vec3 halfwayDir = normalize(viewDir + lightDir);

        float distance = length(lightPositions[i] - fsIn.fragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF(双向反射分布函数模型)，镜面反射部分
        float NDF = DistributionGGX(normal, halfwayDir, roughness);   
        float G = GeometrySmith(normal, viewDir, lightDir, roughness);      
        vec3 F = FresnelSchlick(clamp(dot(halfwayDir, viewDir), 0.0, 1.0), f0);
           
        vec3 numerator    = NDF * G * F; 
        // 入射（光）方向ωi，出射（观察）方向ωo
        float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        

        // kS is equal to Fresnel
        vec3 kS = F;// 镜面分量
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS; // 漫反射分量
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float ndotL = max(dot(normal, lightDir), 0.0);        

        // add to outgoing radiance Lo(出射光线)
        Lo += (kD * albedo / PI + specular) * radiance * ndotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 kS = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), f0, roughness); 
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(irradianceMap, normal).rgb;
    vec3 diffuse    = irradiance * albedo;
    vec3 ambient    = (kD * diffuse) * ao; 

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0 / 2.2)); 

    FragColor = vec4(color, 1.0);
}

