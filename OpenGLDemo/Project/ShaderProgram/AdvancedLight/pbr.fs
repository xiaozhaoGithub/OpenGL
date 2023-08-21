#version 330 core

out vec4 FragColor;

/**
 * @brief 法线分布函数
 * 从统计学上近似地表示了与某些（半程）向量h取向一致的微平面的比率。举例来说，假设给定向量h,如果我们的微平面中有35%与向量h取向一致，
 * 则法线分布函数或者说NDF将会返回0.35。
 * 
 * @param n 法向量
 * @param h 用来与平面上微平面做比较用的半程向量
 * @param a 表面粗糙度
 *
 * @return 返回与向量h取向一致的微平面的比率
 */
float D_GGX_TR(vec3 n, vec3 h, float a) 
{
    float a2 = a * a;
    float nDotH = max(dot(n, h), 0.0);
    float nDotH2 = nDotH * nDotH;
    
    float nom = a2;

    float denom = (nDotH2 * (a2 - 1.0) + 1)
    denom = PI * denom * denom;

    return nom / denom;
}

/**
 * @brief 几何函数
 * 从统计学上近似的求得了微平面间相互遮蔽的比率，这种相互遮蔽会损耗光线的能量。粗糙度较高的表面其微平面间相互遮蔽的概率就越高
 * 
 * @param n 法向量
 * @param k k是α的重映射(Remapping)，直接光照和IBL光照计算公式不同
 *
 * @return 遮蔽的比率
 */
 float GeometrySchlickGGX(float nDotV, float k)
 {
     float nom = nDotV;
     float denom = nDotV * (1.0 - k) + k;

     return nom / denom;
 }

 float GeometrySmith(vec3 n, vec3 v, vec3 l, float k)
 {
     float nDotV = max(dot(n, v), 0.0);
     float nDotL = max(dot(n, l), 0.0);
     float ggx1 = GeometrySchlickGGX(nDotV, k);
     float ggx2 = GeometrySchlickGGX(nDotL, k);

    // 利用史密斯法将观察方向（几何遮蔽(Geometry Obstruction)）和光线方向向量（几何阴影(Geometry Shadowing)）都考虑进去
     return ggx1 * ggx2;
 }

/**
 * @brief 菲涅尔方程
 * 
 * @param f0 表示平面的基础反射率，它是利用所谓折射指数(Indices of Refraction)或者说IOR计算得出的
 * @param cosTheta h dot n or v dor n.
 *
 * @return 返回被反射的光线对比光线被折射的部分所占的比率
 */
vec3 FresnelSchlick(float cosTheta, vec3 f0) 
{
    return f0 + (1 - f0) * pow((1 - cosTheta), 5.0);
}

void main()
{
    vec3 f0 = vec3(0.04);
    f0 = mix(F0, surfaceColor.rgb, metalness); // 金属需要混合
    // FresnelSchlick();

    FragColor = vec4(1.0);
}

