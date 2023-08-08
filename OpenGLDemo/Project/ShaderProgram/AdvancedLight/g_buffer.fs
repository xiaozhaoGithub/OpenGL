#version 330 core

layout(location=0)out vec3 gPosition;
layout(location=1)out vec3 gNormal;
layout(location=2)out vec4 gAlbedoSpec;

in VsOut{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
}fsIn;

struct Material{
	sampler2D diffuse1;
	sampler2D specular1;
};

uniform Material material;

void main()
{
	gPosition=fsIn.fragPos;
	gNormal=normalize(fsIn.normal);
	
	gAlbedoSpec.rgb = texture(material.diffuse1, fsIn.texCoords).rgb;
	gAlbedoSpec.a=texture(material.specular1,fsIn.texCoords).r;
}

