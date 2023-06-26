#version 330 core
out vec4 FragColor;

void main()
{             
	// 底层会自动设置深度缓冲，无需手动赋值，提高效率
    // gl_FragDepth = gl_FragCoord.z;
}