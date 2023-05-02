#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;// 反光度越大，光源越小越亮
};

// 一个光源对它的ambient、diffuse和specular光照分量有着不同的强度。环境光照通常被设置为一个比较低的强度，因为我们不希望环境光颜色太过主导。
// 光源的漫反射分量通常被设置为我们希望光所具有的那个颜色，通常是一个比较明亮的白色。镜面光分量通常会保持为vec3(1.0)，以最大强度发光
struct LightAttr {
	vec3 positon;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform LightAttr lightAttr;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 targetColor;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 normalVector;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

// 片段着色器里的计算都是在世界空间坐标中进行的
// 目前是在世界空间中计算冯氏光照，也可在观察空间（而不是世界空间），称为高氏着色，在顶点着色器中做光照的优势是，相比片段来说，顶点要少得多，因此会更高效。其他插值
void main()
{
	// 环境光照分量
	vec3 ambient = lightAttr.ambient * material.ambient;
	vec3 ambientColor = lightColor * ambient;
	
	// 漫反射分量
	vec3 lightDirection = normalize(lightPos - FragPos); // 法向量方向是从物体表面垂直向外，此处方向目标到光源
	float diffuseFactor = max(dot(lightDirection, normalize(normalVector)), 0.0);
	vec3 diffuse = lightAttr.diffuse * (diffuseFactor * material.diffuse);
	vec3 diffuseColor = lightColor * diffuse;
	
	// 镜面光照分量
	vec3 reflectDirection = reflect(-lightDirection, normalize(normalVector)); // 第一个参数必须是光源到目标的方向
	vec3 viewDirection = normalize(viewPos - FragPos);
	
	float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = lightAttr.specular * (specularFactor * material.specular);
	vec3 specularColor = lightColor * specular;
	
	vec3 result = (ambientColor + diffuseColor + specularColor) * targetColor;

	FragColor = vec4(result, 1.0f);
}	