#version 420 core

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{	
	float distance = length(fragPos.xyz - lightPos);
	// map to [0-1]
	distance /= farPlane;
	
	gl_FragDepth = distance;
}	


	

