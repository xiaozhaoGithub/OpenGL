#version 330 core

layout(location=0)out vec3 gPosition;
layout(location=1)out vec3 gNormal;
layout(location=2)out vec3 gAlbedo;

in VsOut{
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
}fsIn;

void main()
{
	gPosition = fsIn.fragPos;
	gNormal = normalize(fsIn.normal);
	gAlbedo.rgb = vec3(0.95);
}

