#version 330 core

in vec2 texturePos;
out float FragColor;

uniform sampler2D gSsao;

void main()
{
	vec2 texelSize = vec2(textureSize(gSsao, 0));

	float result = 0.0;
	for(int x = -2; x < 2; ++x) {
		for(int y = -2; y < 2; ++y) {
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(gSsao, texturePos + offset).r;
		}
	}

	// avg
	FragColor = result / (4.0 * 4.0);
}

