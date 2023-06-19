#version 330 core

in VsOut {
	vec2 texturePos;
	vec3 fragPos;
	vec3 normal;
} vsIn;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform sampler2D sampler1;

void main()
{	
	vec3 viewDir = normalize(viewPos - vsIn.fragPos);
	vec3 lightDir = normalize(lightPos - vsIn.fragPos);
	vec3 halfwayDir = normalize(viewDir + lightDir);
	
	vec3 lightColor = pow(max(dot(vsIn.normal, halfwayDir), 0), 2)
	
	vec4 texColor = texture(sampler1, vsIn.texturePos);
	
	FragColor = texColor + lightColor;
}	


	

