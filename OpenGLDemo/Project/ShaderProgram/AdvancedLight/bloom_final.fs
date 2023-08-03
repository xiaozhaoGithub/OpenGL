#version 330 core

out vec4 FragColor;
in vec2 texturePos;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool isBloom;
uniform float exposure;

void main()
{	
	const float gamma = 2.2;
	vec3 hdrColor = texture(scene, texturePos).rgb;
	vec3 bloomColor = texture(bloomBlur, texturePos).rgb;
	
	if(isBloom) {
		hdrColor += bloomColor;
	}
	
	vec3 result = 1.0 - exp(-hdrColor * exposure);
	result = pow(result, vec3(1.0 / gamma));
	
	FragColor = vec4(result, 1.0);
}	


	

