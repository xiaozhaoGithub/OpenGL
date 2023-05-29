#version 420 core // 注意GLSL的版本！（4.2.0）

uniform sampler2D sampler1;

in VS_OUT 
{
	vec2 texturePos;
} fs_in;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

// OpenGL就能假设你只会写入比当前片段深度值更大的值
// 当深度值比片段的深度值要小的时候，OpenGL仍是能够进行提前深度测试
layout(depth_greater) out float gl_FragDepth;

void main()
{	
	// 正背向面输出不同效果
	if(gl_FrontFacing) {
		if(gl_FragCoord.x < 600) {
			FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		}
		else {
			FragColor = vec4(0.0, 1.0, 0.0, 1.0);
		}
	}
	else {
		
		FragColor = vec4(texture(sampler1, fs_in.texturePos));
	}
	
	gl_FragDepth = gl_FragCoord.z + 0.01; 
}	


	

