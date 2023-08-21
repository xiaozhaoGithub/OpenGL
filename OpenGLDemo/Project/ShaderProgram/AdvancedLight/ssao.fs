#version 330 core

out float FragColor; // float灰度输出，生成遮蔽因子

in vec2 texturePos;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gTexNoise; // 沿Z轴的随机旋转向量

uniform mat4 projectionMat;
uniform vec3 samples[64];

// tile noise texture over screen based on screen dimensions divided by noise size
// UCDD::kViewportWidth / 4.0f, UCDD::kViewportHeight / 4.0f
// kViewportWidth / 4.0f个纹理才能平铺满屏幕
uniform vec2 noiseScale;

void main()
{
    vec3 fragPos = texture(gPosition, texturePos).rgb;
    vec3 normal = texture(gNormal, texturePos).rgb;
    vec3 randomVec = texture(gTexNoise, texturePos * noiseScale).rgb; // texturePos和gTexNoise非对应，缩放坐标GL_REPEAT采样 > 1.0的坐标
    
    // 创建TBN矩阵，from tangent-space to view-space
    // 正交基的叉乘就是副切线
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal)); // ???
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // 迭代采样核并计算遮蔽因子
    int kernelSize = 64;
    float radius = 0.5;
    float bias = 0.025;

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i) {
        // get sample position
        // 随机偏移的TBN * 随机半核法向样本后，得到经转动的随机观察坐标
        vec3 samplePos = TBN * samples[i]; // from tangent-space to view-space
        samplePos = fragPos + samplePos * radius;

        // project sample position
        vec4 sampleProjectPos = vec4(samplePos, 1.0);
        sampleProjectPos *= projectionMat; // from view to clip-space
        sampleProjectPos.xyz /= sampleProjectPos.w; // perspective divide
        sampleProjectPos.xyz = sampleProjectPos.xyz * 0.5 + 0.5; // transform range to 0.0 - 1.0

        // get sample depth
        // 观察位置纹理，通过裁剪坐标获取样本的深度值 ???
        float sampleDepth = texture(gPosition, sampleProjectPos.xy).z; // get depth value of kernel sample（半向法球中的采样点）

        // range check && caculate
        // 当检测一个靠近表面边缘的片段时，它将会考虑测试表面之下的表面的深度值；这些值将会(不正确地)影响遮蔽因子，引入一个范围检测从而解决这个问题
        // 0-1平滑插值
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth)); // 深度差在radius内，才会用于计算？？？
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck; // 深度差在radius内，且差越大，所占黑暗浙滨因子分量越大
    }
    occlusion = 1.0 - (occlusion / kernelSize); // 用于环境光的遮蔽因子（亮的占比）
    
    FragColor = occlusion;
}

