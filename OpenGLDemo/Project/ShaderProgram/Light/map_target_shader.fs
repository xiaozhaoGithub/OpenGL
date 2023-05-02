#version 330 core

struct Material {
	//vec3 ambient; //漫反射贴图，材质中，环境光照应与漫反射一致，此处删除该变量
	sampler2D diffuse;// Note
	sampler2D specular;// Note
	sampler2D emission;// Note
	float shininess;// 反光度越大，光源越小越亮
};

// 一个光源对它的ambient、diffuse和specular光照分量有着不同的强度。环境光照通常被设置为一个比较低的强度，因为我们不希望环境光颜色太过主导。
// 光源的漫反射分量通常被设置为我们希望光所具有的那个颜色，通常是一个比较明亮的白色。镜面光分量通常会保持为vec3(1.0)，以最大强度发光
struct LightAttr {
	vec3 position;// 根据第4个分量判断（1.0为位置光可平移，0.0为平行光，只有方向不可平移）
	vec3 direction; //平行光(如太阳光)直接用光的方向向量，位置太远，以至可忽略
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	// 光衰减系数
	float constant;
	float linear;
	float quadratic;
};

struct FlashlightAttr {
	vec3 position;
	vec3 direction;
	float cutoff;
};

uniform Material material;
uniform LightAttr lightAttr;
uniform FlashlightAttr flashLightAttr;

uniform vec3 lightColor;
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 normalVector;
in vec2 texturePos;

out vec4 FragColor; // 片段着色器唯一输出，输出片段颜色

// 片段着色器里的计算都是在世界空间坐标中进行的
// 目前是在世界空间中计算冯氏光照，也可在观察空间（而不是世界空间），称为高氏着色，在顶点着色器中做光照的优势是，相比片段来说，顶点要少得多，因此会更高效。其他插值
void main()
{	
	// 环境光照分量
	vec3 ambient = lightAttr.ambient * vec3(texture(material.diffuse, texturePos));// 不要忘记将环境光的材质颜色设置为漫反射材质颜色同样的值。
	vec3 ambientColor = lightColor * ambient;
	
	// 漫反射分量
	vec3 lightDirection = normalize(lightAttr.position - FragPos); // 法向量方向是从物体表面垂直向外，此处方向目标到光源
	//vec3 lightDirection = normalize(-lightAttr.direction); // 法向量方向是从物体表面垂直向外，此处方向目标到光源
	
	float diffuseFactor = max(dot(lightDirection, normalize(normalVector)), 0.0);
	vec3 diffuse = lightAttr.diffuse * (diffuseFactor * vec3(texture(material.diffuse, texturePos)));// 漫反射贴图
	vec3 diffuseColor = lightColor * diffuse;

	// 镜面光照分量
	vec3 reflectDirection = reflect(-lightDirection, normalize(normalVector)); // 第一个参数必须是光源到目标的方向
	vec3 viewDirection = normalize(viewPos - FragPos);
	
	float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	// 镜面高光的强度可以通过纹理图像每个像素的亮度来获取，个像素越「白」，乘积就会越大，物体的镜面光分量就会越亮
	vec3 specular = lightAttr.specular * (specularFactor * vec3(texture(material.specular, texturePos)));// 镜面光照贴图
	vec3 specularColor = lightColor * specular;

	
	// 放射光分量（如机器人眼睛、箱子的灯带）
	vec3 emissionColor = texture(material.emission, texturePos).rgb; // 放射光贴图
	
	// 衰减因子
	float distance = length(lightAttr.position - FragPos);
	float attenuation = 1.0 / (lightAttr.constant + lightAttr.linear * distance + lightAttr.quadratic * (distance * distance));

	ambientColor *= attenuation; // 使用多于一个的光源，所有的环境光分量将会开始叠加，所以在这种情况下我们也希望衰减环境光照
	diffuseColor *= attenuation;
	specularColor *= attenuation;


	vec3 result = ambientColor + diffuseColor + specularColor; //纹理图片不需要乘目标颜色，不存在
	FragColor = vec4(result, 1.0f);
}	