#version 330 core
out vec4 FragColor;

in vec2 texturePos;

uniform bool isHorizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162); // 纹素权重

uniform sampler2D brightTexture;

void main()
{             
	vec2 texOffset = 1.0 / textureSize(brightTexture, 0);
	vec3 result = texture(brightTexture, texturePos).rgb * weight[0];
	if(isHorizontal) {
		for(int i = 1; i < 5; i++) {
			result += texture(brightTexture, texturePos + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
			result += texture(brightTexture, texturePos - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
		} 
	}
	else {
		for(int i = 1; i < 5; i++) {
			result += texture(brightTexture, texturePos + vec2(0.0, texOffset.y * i)).rgb * weight[i];
			result += texture(brightTexture, texturePos - vec2(0.0, texOffset.y * i)).rgb * weight[i];
		}
	}
	
	FragColor = vec4(result, 1.0);
}