#version 330 core

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec4 targetColor;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 normalVector;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

// 片段着色器里的计算都是在世界空间坐标中进行的
// 目前是在世界空间中计算冯氏光照，也可在观察空间（而不是世界空间），称为高氏着色，在顶点着色器中做光照的优势是，相比片段来说，顶点要少得多，因此会更高效。其他插值
void main()
{
	// 环境光照分量
	float ambientStrength = 0.1;
	vec4 ambientColor = lightColor * ambientStrength;
	
	// 漫反射分量
	vec3 lightDirection = normalize(lightPos - FragPos); // 法向量方向是从物体表面垂直向外，此处方向目标到光源
	float diffuseFactor = max(dot(lightDirection, normalize(normalVector)), 0.0);
	vec4 diffuseColor = lightColor * diffuseFactor;
	
	// 镜面光照分量
	float specularStrength = 0.5f;
	vec3 reflectDirection = reflect(-lightDirection, normalize(normalVector)); // 第一个参数必须是光源到目标的方向
	vec3 viewDirection = normalize(viewPos - FragPos);
	
	float shininessFactor = 32;// 反光度越大，光源越小越亮
	float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), shininessFactor);
	
	vec4 specularColor = lightColor * specularStrength * specularFactor;
	
	FragColor = (ambientColor + diffuseColor + specularColor) * targetColor;
}	