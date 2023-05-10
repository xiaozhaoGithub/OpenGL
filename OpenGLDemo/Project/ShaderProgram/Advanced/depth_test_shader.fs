#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 texturePos;

// texture sampler
uniform sampler2D sampler1;

float near = 0.1; 
float far  = 100.0; 
float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near)); // 投影矩阵变换而来
}

void main()
{
	// FragColor = texture(sampler1, texturePos);
	
	// 深度测试可视化
	FragColor = vec4(vec3(gl_FragCoord.z), 1.0);// 非线性
	// float depth = LinearizeDepth(gl_FragCoord.z) / far; // 为了演示除以 far，线性
	// FragColor = vec4(vec3(depth), 1.0);
}
