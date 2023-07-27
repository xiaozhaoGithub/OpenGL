#version 330 core

in vec2 texturePos;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform sampler2D hdrFbTexture;
uniform bool isUseHdr;
uniform float exposure; // 曝光度

void main()
{	
	const float gamma = 2.2;
	vec3 hdrColor = texture(hdrFbTexture, texturePos).rgb;
	if(isUseHdr) {
		// reinhard 色调映射
		// vec3 result = hdrColor / (hdrColor + vec3(1.0)); // 变换到0 - 1
		// 曝光 色调映射
		vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
		
		result = pow(result, vec3(1.0 / gamma));
		FragColor = vec4(result, 1.0);
	}
	else {
		vec3 result = pow(hdrColor, vec3(1.0 / gamma));
		FragColor = vec4(result, 1.0);
	}
}	


	

