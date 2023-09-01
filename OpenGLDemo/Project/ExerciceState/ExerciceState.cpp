#include "ExerciceState.h"

#include "ShaderFactory.h"
#include "CommonDataDef.h"

extern GLFWwindow* g_globalWindow;
namespace ESCD = ExerciceStateCommonDataDef;

void AbstractExerciceState::draw()
{
	processInput();
	render();
}

void AbstractExerciceState::initTransformMatUniformBlock()
{
	// Uniform缓冲对象比起独立的uniform有很多好处。
	// 1.一次设置很多uniform会比一个一个设置多个uniform要快很多
	// 2.比起在多个着色器中修改同样的uniform，在Uniform缓冲中修改一次会更容易一些。
	// 3.最后一个好处可能不会立即显现，如果使用Uniform缓冲对象的话，你可以在着色器中使用更多的uniform。
	// OpenGL限制了它能够处理的uniform数量，这可以通过GL_MAX_VERTEX_UNIFORM_COMPONENTS来查询。当使用Uniform缓冲对象时，最大的数量会更高。所以，当你达到了uniform的最大数量时（比如再做骨骼动画(Skeletal Animation)的时候），你总是可以选择使用Uniform缓冲对象。
	glGenBuffers(1, &m_transformMatUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, m_transformMatUbo);
	glBufferData(GL_UNIFORM_BUFFER, 32, NULL, GL_STATIC_DRAW); // 分配152字节的内存
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_transformMatUbo); // 绑定点 0
	//glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboExampleBlock, 0, sizeof(glm::mat4) * 2);
}

void AbstractExerciceState::processInput()
{
	for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; key++) {
		if (glfwGetKey(g_globalWindow, key) == GLFW_PRESS) {
			m_stateKey = key;
		}
	}
}

void AbstractExerciceState::render()
{
}

ClearExerciceState::ClearExerciceState()
{
}

void ClearExerciceState::draw()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

SpecialTriangleExerciceState::SpecialTriangleExerciceState()
{
	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("normal", "ShaderProgram/shader.vs", "ShaderProgram/shader.fs");
	m_specialTriangleVAO = Singleton<TriangleVAOFactory>::instance()->createSpecialVAO();
}

void SpecialTriangleExerciceState::draw()
{
	m_shader->use();
	glBindVertexArray(m_specialTriangleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

AdjacentTriangleExerciceState::AdjacentTriangleExerciceState()
{
	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("normal", "ShaderProgram/shader.vs", "ShaderProgram/shader.fs");
	m_adjacentTriangleVAO = Singleton<TriangleVAOFactory>::instance()->createAdjacentVAO();
}

void AdjacentTriangleExerciceState::draw()
{
	m_shader->use();
	glBindVertexArray(m_adjacentTriangleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

AdjacentTriangle1ExerciceState::AdjacentTriangle1ExerciceState()
{
	m_adjacentTriangle1VAO = Singleton<TriangleVAOFactory>::instance()->createAdjacent2VAO().first;
	m_adjacentTriangle2VAO = Singleton<TriangleVAOFactory>::instance()->createAdjacent2VAO().second;

	m_yellowShaderProgram = Singleton<ShaderFactory>::instance()->shaderProgram("yellow", "ShaderProgram/shader.vs", "ShaderProgram/yellowShader.fs");
}

void AdjacentTriangle1ExerciceState::draw()
{
	glBindVertexArray(m_adjacentTriangle1VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	m_yellowShaderProgram->use();

	glBindVertexArray(m_adjacentTriangle2VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

FlipTriangleExerciceState::FlipTriangleExerciceState()
{
	m_normalTriangleVAO = Singleton<RectVAOFactory>::instance()->createNormalVAO();
	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("flip", "ShaderProgram/flip_shader.vs", "ShaderProgram/shader.fs");
}

void FlipTriangleExerciceState::draw()
{
	m_shader->use();
	m_normalTriangleVAO->bindVAO();
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

RichTriangleExerciceState::RichTriangleExerciceState()
{
	m_richTriangleVAO = Singleton<TriangleVAOFactory>::instance()->createRichVAO();
	m_withColorShaderProgram = Singleton<ShaderFactory>::instance()->shaderProgram("withColor", "ShaderProgram/vertex_with_color_shader.vs", "ShaderProgram/vertex_with_color_shader.fs");
}

void RichTriangleExerciceState::draw()
{
	m_withColorShaderProgram->use();
	m_withColorShaderProgram->setFloat("xOffset", 0.5f);
	glBindVertexArray(m_richTriangleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

RectExerciceState::RectExerciceState()
{
	m_rectVAO = Singleton<RectVAOFactory>::instance()->createNormalVAO();
	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("normal", "ShaderProgram/shader.vs", "ShaderProgram/shader.fs");
}

void RectExerciceState::draw()
{
	m_shader->use();
	m_rectVAO->bindVAO();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

LineRectExerciceState::LineRectExerciceState()
	: RectExerciceState()
{
}

void LineRectExerciceState::draw()
{
	m_shader->use();
	m_rectVAO->bindVAO();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

TextureExerciceState::TextureExerciceState()
{
	m_textureTriangleVAO = Singleton<TriangleVAOFactory>::instance()->creatTextureVAO();
	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("texture", "ShaderProgram/Texture/texture_shader.vs", "ShaderProgram/Texture/texture_shader.fs");
}

void TextureExerciceState::draw()
{
	m_shader->use();

	m_textureTriangleVAO->bindVAO();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

TextureMixExerciceState::TextureMixExerciceState()
{
	m_textureUnitTriangleVAO = Singleton<TriangleVAOFactory>::instance()->creatMixTextureUnitVAO();

	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("texture_unit", "ShaderProgram/Texture/texture_shader.vs", "ShaderProgram/Texture/texture_unit_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0); // 使用着色器程序后，才能对uniform变量赋值
	m_shader->setInt("texture2", 1);
}

void TextureMixExerciceState::draw()
{
	m_shader->use();
	m_textureUnitTriangleVAO->bindTexture();
	m_textureUnitTriangleVAO->bindVAO();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

TextureExercice1State::TextureExercice1State()
{
	m_textureUnitTriangleVAO = Singleton<TriangleVAOFactory>::instance()->creatMixTextureUnitVAO();

	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("exercise_1_shader", "ShaderProgram/Texture/texture_shader.vs", "ShaderProgram/Texture/exercise_1_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0); // 使用着色器程序后，才能对uniform变量赋值, 设置着色器中的采样器的位置索引
	m_shader->setInt("texture2", 1);
}

void TextureExercice1State::draw()
{
	m_shader->use();
	m_textureUnitTriangleVAO->bindTexture();
	m_textureUnitTriangleVAO->bindVAO();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

TextureExercice4State::TextureExercice4State()
	: m_opacity(0.5f)
{
	m_textureUnitTriangleVAO = Singleton<TriangleVAOFactory>::instance()->creatMixTextureUnitVAO();

	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("exercise_4_shader", "ShaderProgram/Texture/texture_shader.vs", "ShaderProgram/Texture/exercise_4_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0);
	m_shader->setInt("texture2", 1);
	m_shader->setFloat("opacity", m_opacity);
}

void TextureExercice4State::draw()
{
	m_shader->use();

	m_textureUnitTriangleVAO->bindTexture();
	m_textureUnitTriangleVAO->bindVAO();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	if (glfwGetKey(g_globalWindow, GLFW_KEY_UP) == GLFW_PRESS) {
		m_opacity += 0.01f;
		if (m_opacity >= 1.0f) {
			m_opacity = 1.0f;
		}
		m_shader->setFloat("opacity", m_opacity);
	}
	if (glfwGetKey(g_globalWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
		m_opacity -= 0.01f;
		if (m_opacity <= 0.0f) {
			m_opacity = 0.0f;
		}
		m_shader->setFloat("opacity", m_opacity);
	}
}

TransformTextureExerciceState::TransformTextureExerciceState()
{
	m_textureTransformTriangleVAO = Singleton<TriangleVAOFactory>::instance()->createTransformVAO();

	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("transfrom_shader", "ShaderProgram/Texture/texture_transform_shader.vs", "ShaderProgram/Texture/texture_unit_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0);
	m_shader->setInt("texture2", 1);

	glm::mat4 transformMatrix;
	transformMatrix = glm::rotate(transformMatrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	transformMatrix = glm::scale(transformMatrix, glm::vec3(0.5, 0.5, 0.5));
	m_shader->setMatrix("transformMatrix", glm::value_ptr(transformMatrix));
}

void TransformTextureExerciceState::draw()
{
	m_shader->use();

	glm::mat4 transfromMat;
	float sec = (float)glfwGetTime(); // glfw运行程序的累加时间，相当于Timer，可重置时间
	transfromMat = glm::translate(transfromMat, glm::vec3(0.5f, -0.5f, 0.0f));// 顺序不一致，效果不一致
	transfromMat = glm::rotate(transfromMat, sec, glm::vec3(0.0f, 0.0f, 1.1f));
	m_shader->setMatrix("transformMatrix", glm::value_ptr(transfromMat));

	m_textureTransformTriangleVAO->bindTexture();
	m_textureTransformTriangleVAO->bindVAO();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	transfromMat = glm::mat4(1.0f);
	transfromMat = glm::translate(transfromMat, glm::vec3(-0.5f, 0.5f, 0.0f));

	float scaleValue = abs(sin(sec));
	transfromMat = glm::scale(transfromMat, glm::vec3(scaleValue, scaleValue, scaleValue));
	m_shader->setMatrix("transformMatrix", &transfromMat[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

Fake3DTextureExerciceState::Fake3DTextureExerciceState()
{
	m_textureFake3DTriangleVAO = Singleton<TriangleVAOFactory>::instance()->createTransformVAO();

	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("fake_3d_shader", "ShaderProgram/Texture/texture_3d_shader.vs", "ShaderProgram/Texture/texture_unit_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0);
	m_shader->setInt("texture2", 1);

	glm::mat4 transformMatrix;
	transformMatrix = glm::rotate(transformMatrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	transformMatrix = glm::scale(transformMatrix, glm::vec3(0.5, 0.5, 0.5));
	m_shader->setMatrix("transformMatrix", glm::value_ptr(transformMatrix));
}

void Fake3DTextureExerciceState::draw()
{
	m_shader->use();

	m_textureFake3DTriangleVAO->bindTexture();
	m_textureFake3DTriangleVAO->bindVAO();

	glm::mat4 modelMat;
	modelMat = glm::rotate(modelMat, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));

	glm::mat4 viewMat;
	viewMat = glm::translate(viewMat, glm::vec3(0.0f, 0.0f, -3.0f));
	m_shader->setMatrix("viewMat", glm::value_ptr(viewMat));

	glm::mat4 projectionMat;
	projectionMat = glm::perspective(glm::radians(45.0f), (float)(800.0 / 600.0), 0.1f, 100.0f);
	m_shader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

CubeTextureExerciceState::CubeTextureExerciceState()
{
	m_texture3DTriangleVAO = Singleton<TriangleVAOFactory>::instance()->create3DVAO();

	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("cube_shader", "ShaderProgram/Texture/cube_3d_shader.vs", "ShaderProgram/Texture/texture_unit_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0);
	m_shader->setInt("texture2", 1);
}

void CubeTextureExerciceState::draw()
{
	Singleton<CameraWrapper>::instance()->setCursorVisible(false);
	m_shader->use();

	m_texture3DTriangleVAO->bindTexture();
	m_texture3DTriangleVAO->bindVAO();

	//glm::mat4 viewMat;
	//viewMat = glm::translate(viewMat, glm::vec3(0.0f, -0.0f, -3.0f));
	//m_shader->setMatrix("viewMat", glm::value_ptr(viewMat));

	for (int i = 0; i < 10; ++i) {
		glm::mat4 modelMat;
		modelMat = glm::translate(modelMat, ESCD::cubePositions[i]);

		if (i == 0 || i % 3 == 0) {
			float sec = (float)glfwGetTime();
			modelMat = glm::rotate(modelMat, sec, glm::vec3(1.0f, 0.0f, 0.0f));
			m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));
		}
		else {
			m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));
		}

		//float radius = 10.0f;
		//float camX = (float)sin(glfwGetTime()) * radius;
		//float camZ = (float)cos(glfwGetTime()) * radius;

		//Singleton<CameraWrapper>::instance()->processKeyPress();
		//auto viewMat = Singleton<CameraWrapper>::instance()->lookAtMatrix();
		auto viewMat = Singleton<CameraWrapper>::instance()->customLookAtMatrix();
		m_shader->setMatrix("viewMat", glm::value_ptr(viewMat)); // 将顶点变换到观察矩阵的坐标空间

		glm::mat4 projectionMat;
		//glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);正射投影矩阵
		// 透视投影矩阵，距离越远w其次分量越大，vertex/w 顶点越小，虚拟远近效果
		// 生成平截头体
		//projectionMat = glm::perspective(glm::radians(45.0f), float(800) / (float)600, 0.1f, 100.0f);
		projectionMat = glm::perspective(glm::radians(Singleton<CameraWrapper>::instance()->fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
		m_shader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

LightSceneExerciceState::LightSceneExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
	, m_targetColor(glm::vec4(1.0f, 0.5f, 0.31f, 1.0f))
{
	m_lightVAO = Singleton<TriangleVAOFactory>::instance()->createLightVAO();
	m_targetVAO = Singleton<TriangleVAOFactory>::instance()->createTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = Singleton<ShaderFactory>::instance()->shaderProgram("light_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = Singleton<ShaderFactory>::instance()->shaderProgram("target_shader", "ShaderProgram/Light/target_shader.vs", "ShaderProgram/Light/target_shader.fs");
	
	m_lightShader->use();
	// 常见问题：着色器中变量类型与设置时不匹配时，片段着色器输出黑色，如vec4未设置第四个分量
	m_lightShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b, m_lightColor.a);

	m_targetShader->use(); 
	m_targetShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b);
	m_targetShader->setVec("lightPos", m_lightPos.x, m_lightPos.y, m_lightPos.z);
	m_targetShader->setVec("targetColor", m_targetColor.r, m_targetColor.g, m_targetColor.b);
	//m_targetShader->setVec("material.ambient", 1.0f, 0.5f, 0.31f);
	//m_targetShader->setVec("material.diffuse", 1.0f, 0.5f, 0.31f);
	//m_targetShader->setVec("material.specular", 0.5f, 0.5f, 0.5f);
	m_targetShader->setVec("material.ambient", 0.0f, 0.1f, 0.06f);
	m_targetShader->setVec("material.diffuse", 0.0f, 0.50980392f, 0.50980392f);
	m_targetShader->setVec("material.specular", 0.50196078f, 0.50196078f, 0.50196078f);
	m_targetShader->setFloat("material.shininess", 32.0f);
	m_targetShader->setVec("lightAttr.position", m_lightPos.x, m_lightPos.y, m_lightPos.z);
	//m_targetShader->setVec("lightAttr.ambient", 0.2f, 0.2f, 0.2f);
	//m_targetShader->setVec("lightAttr.diffuse", 0.5f, 0.5f, 0.5f);
	//m_targetShader->setVec("lightAttr.specular", 1.0f, 1.0f, 1.0f);
	
	// 注意材质表格中的环境光值与漫反射值不一样，它们没有考虑光照的强度。要想正确地设置它们的值，你需要将所有的光照强度都设置为vec3(1.0)
	m_targetShader->setVec("lightAttr.ambient", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("lightAttr.diffuse", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("lightAttr.specular", 1.0f, 1.0f, 1.0f);

	Singleton<CameraWrapper>::instance()->setCursorVisible(false);
}


void LightSceneExerciceState::draw()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_lightShader->use();
	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置
	m_lightPos.x = 1.0f + sin((float)glfwGetTime()) * 2.0f;// 值域 -3 到 3
	m_lightPos.y = sin((float)glfwGetTime() / 2.0f) * 1.0f;// -1 到 1

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, m_lightPos);
	modelMat = glm::scale(modelMat, glm::vec3(0.2f));
	m_lightShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	glm::mat4 viewMat = Singleton<CameraWrapper>::instance()->lookAtMatrix();
	m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

	// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
	glm::mat4 projectionMat = glm::perspective(glm::radians(Singleton<CameraWrapper>::instance()->fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
	m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

	m_lightVAO->bindVAO();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	m_targetShader->use();
	modelMat = glm::mat4(1.0f);
	m_targetShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
	m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));
	auto cameraPos = Singleton<CameraWrapper>::instance()->cameraPos();
	m_targetShader->setVec("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
	m_targetShader->setVec("lightPos", m_lightPos.x, m_lightPos.y, m_lightPos.z);
	m_targetShader->setVec("lightAttr.position", m_lightPos.x, m_lightPos.y, m_lightPos.z);

	// 模拟不同光源
	/*glm::vec4 lightColor;
	lightColor.x = sin(glfwGetTime() * 2.0f);
	lightColor.y = sin(glfwGetTime() * 0.7f);
	lightColor.z = sin(glfwGetTime() * 1.3f);
	m_lightColor = lightColor;
	m_lightShader->use();
	m_lightShader->setVec("lightColor", m_lightColor);

	m_targetShader->use();
	m_targetShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b);*/

	m_targetVAO->bindVAO();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

LightMapExerciceState::LightMapExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	m_lightVAO = Singleton<TriangleVAOFactory>::instance()->createLightVAO();
	m_targetVAO = Singleton<TriangleVAOFactory>::instance()->createLightMapTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = Singleton<ShaderFactory>::instance()->shaderProgram("texture_light_map_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = Singleton<ShaderFactory>::instance()->shaderProgram("texture_target_map_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Light/texture_map_target_shader.fs");

	m_lightShader->use();
	// 常见问题：着色器中变量类型与设置时不匹配时，片段着色器输出黑色，如vec4未设置第四个分量
	m_lightShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b, m_lightColor.a);

	m_targetShader->use();
	m_targetShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b);

	m_targetShader->setInt("material.diffuse", 0); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_targetShader->setInt("material.specular", 1); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_targetShader->setInt("material.emission", 2); // note 放射光分量
	m_targetShader->setFloat("material.shininess", 64.0f);

	m_targetShader->setVec("lightAttr.position", m_lightPos.x, m_lightPos.y, m_lightPos.z);
	//m_targetShader->setVec("lightAttr.ambient", 0.2f, 0.2f, 0.2f);
	m_targetShader->setVec("lightAttr.ambient", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("lightAttr.diffuse", 0.5f, 0.5f, 0.5f);
	m_targetShader->setVec("lightAttr.specular", 1.0f, 1.0f, 1.0f);

	Singleton<CameraWrapper>::instance()->setCursorVisible(false);
}

void LightMapExerciceState::draw()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_lightShader->use();
	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置
	m_lightPos.x = 1.0f + sin((float)glfwGetTime()) * 2.0f;// 值域 -3 到 3
	m_lightPos.y = sin((float)glfwGetTime() / 2.0f) * 1.0f;// -1 到 1

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, m_lightPos);
	modelMat = glm::scale(modelMat, glm::vec3(0.2f));
	m_lightShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	glm::mat4 viewMat = Singleton<CameraWrapper>::instance()->lookAtMatrix();
	m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

	// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
	glm::mat4 projectionMat = glm::perspective(glm::radians(Singleton<CameraWrapper>::instance()->fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
	m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

	m_lightVAO->bindVAO();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	m_targetShader->use();
	modelMat = glm::mat4(1.0f);
	m_targetShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
	m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));
	auto cameraPos = Singleton<CameraWrapper>::instance()->cameraPos();
	m_targetShader->setVec("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
	m_targetShader->setVec("lightAttr.position", m_lightPos.x, m_lightPos.y, m_lightPos.z);

	m_targetVAO->bindVAO();
	m_targetVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

LightSourceExerciceState::LightSourceExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	m_lightVAO = Singleton<TriangleVAOFactory>::instance()->createLightVAO();
	m_targetVAO = Singleton<TriangleVAOFactory>::instance()->createLightMapTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = Singleton<ShaderFactory>::instance()->shaderProgram("cast_light_map_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = Singleton<ShaderFactory>::instance()->shaderProgram("cast_target_map_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Light/map_target_shader.fs");

	m_lightShader->use();
	// 常见问题：着色器中变量类型与设置时不匹配时，片段着色器输出黑色，如vec4未设置第四个分量
	m_lightShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b, m_lightColor.a);

	m_targetShader->use();
	m_targetShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b);

	m_targetShader->setInt("material.diffuse", 0); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_targetShader->setInt("material.specular", 1); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_targetShader->setInt("material.emission", 2); // note 放射光分量
	m_targetShader->setFloat("material.shininess", 64.0f);

	m_targetShader->setVec("lightAttr.position", m_lightPos.x, m_lightPos.y, m_lightPos.z);
	m_targetShader->setVec("lightAttr.direction", -0.2f, -1.0f, -0.3f);// 定向光方向向量
	m_targetShader->setVec("lightAttr.ambient", 0.2f, 0.2f, 0.2f);
	m_targetShader->setVec("lightAttr.diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("lightAttr.specular", 1.0f, 1.0f, 1.0f);
	// 衰减系数
	m_targetShader->setFloat("lightAttr.constant", 1.0f);
	m_targetShader->setFloat("lightAttr.linear", 0.09f);
	m_targetShader->setFloat("lightAttr.quadratic", 0.032f);

	Singleton<CameraWrapper>::instance()->setCursorVisible(false);
}

void LightSourceExerciceState::draw()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_lightShader->use();
	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置
	m_lightPos.x = 1.0f + sin((float)glfwGetTime()) * 2.0f;// 值域 -3 到 3
	m_lightPos.y = sin((float)glfwGetTime() / 2.0f) * 1.0f;// -1 到 1

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, m_lightPos);
	modelMat = glm::scale(modelMat, glm::vec3(0.2f));
	m_lightShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	glm::mat4 viewMat = Singleton<CameraWrapper>::instance()->lookAtMatrix();
	m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

	// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
	glm::mat4 projectionMat = glm::perspective(glm::radians(Singleton<CameraWrapper>::instance()->fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
	m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

	m_lightVAO->bindVAO();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	m_targetShader->use();
	m_targetShader->setVec("lightAttr.position", Singleton<CameraWrapper>::instance()->cameraPos());
	m_targetShader->setVec("viewPos", Singleton<CameraWrapper>::instance()->cameraPos());

	m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
	m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

	for (unsigned int i = 0; i < 10; i++) {
		glm::mat4 modelMat;
		modelMat = glm::translate(modelMat, ESCD::cubePositions[i]);
		float angle = 20.0f * i;
		modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_targetShader->setMatrix("modelMat", glm::value_ptr(modelMat));

		m_targetVAO->bindVAO();
		m_targetVAO->bindTexture();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

FlashLightSourceExerciceState::FlashLightSourceExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	m_lightVAO = Singleton<TriangleVAOFactory>::instance()->createLightVAO();
	m_targetVAO = Singleton<TriangleVAOFactory>::instance()->createLightMapTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = Singleton<ShaderFactory>::instance()->shaderProgram("flash_light_light_map_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = Singleton<ShaderFactory>::instance()->shaderProgram("flash_light_target_map_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Light/flash_light_target_shader.fs");

	m_lightShader->use();
	// 常见问题：着色器中变量类型与设置时不匹配时，片段着色器输出黑色，如vec4未设置第四个分量
	m_lightShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b, m_lightColor.a);

	m_targetShader->use();
	m_targetShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b);

	m_targetShader->setInt("material.diffuse", 0); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_targetShader->setInt("material.specular", 1); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_targetShader->setInt("material.emission", 2); // note 放射光分量
	m_targetShader->setFloat("material.shininess", 32.0f);

	// 聚光手电筒
	m_targetShader->setVec("lightAttr.position", Singleton<CameraWrapper>::instance()->cameraPos());
	m_targetShader->setVec("lightAttr.direction", Singleton<CameraWrapper>::instance()->cameraFrontPos());
	m_targetShader->setFloat("lightAttr.cutoff", glm::cos(glm::radians(5.5f))); // 切光角
	m_targetShader->setFloat("lightAttr.outerCutoff", glm::cos(glm::radians(17.5f))); // 切光角

	m_targetShader->setVec("lightAttr.ambient", 0.1f, 0.1f, 0.1f);
	m_targetShader->setVec("lightAttr.diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("lightAttr.specular", 1.0f, 1.0f, 1.0f);
	// 衰减系数
	m_targetShader->setFloat("lightAttr.constant", 1.0f);
	m_targetShader->setFloat("lightAttr.linear", 0.09f);
	m_targetShader->setFloat("lightAttr.quadratic", 0.032f);

	Singleton<CameraWrapper>::instance()->setCursorVisible(false);
}

void FlashLightSourceExerciceState::draw()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_lightShader->use();
	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置
	m_lightPos.x = 1.0f + sin((float)glfwGetTime()) * 2.0f;// 值域 -3 到 3
	m_lightPos.y = sin((float)glfwGetTime() / 2.0f) * 1.0f;// -1 到 1

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, m_lightPos);
	modelMat = glm::scale(modelMat, glm::vec3(0.2f));
	m_lightShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	glm::mat4 viewMat = Singleton<CameraWrapper>::instance()->lookAtMatrix();
	m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

	// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
	glm::mat4 projectionMat = glm::perspective(glm::radians(Singleton<CameraWrapper>::instance()->fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
	m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

	//m_lightVAO->bindVAO();
	//glDrawArrays(GL_TRIANGLES, 0, 36);

	m_targetShader->use();
	// 聚光手电筒
	m_targetShader->setVec("lightAttr.position", Singleton<CameraWrapper>::instance()->cameraPos());
	m_targetShader->setVec("lightAttr.direction", Singleton<CameraWrapper>::instance()->cameraFrontPos());

	m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
	m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));
	m_targetShader->setVec("viewPos", Singleton<CameraWrapper>::instance()->cameraPos());

	for (unsigned int i = 0; i < 10; i++) {
		glm::mat4 modelMat;
		modelMat = glm::translate(modelMat, ESCD::cubePositions[i]);
		float angle = 20.0f * i;
		modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_targetShader->setMatrix("modelMat", glm::value_ptr(modelMat));

		m_targetVAO->bindVAO();
		m_targetVAO->bindTexture();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

MultipleLightSourceExerciceState::MultipleLightSourceExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	m_lightVAO = Singleton<TriangleVAOFactory>::instance()->createLightVAO();
	m_targetVAO = Singleton<TriangleVAOFactory>::instance()->createLightMapTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = Singleton<ShaderFactory>::instance()->shaderProgram("mult_light_map_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = Singleton<ShaderFactory>::instance()->shaderProgram("mult_target_map_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Light/mult_light_target_shader.fs");

	m_lightShader->use();
	// 常见问题：着色器中变量类型与设置时不匹配时，片段着色器输出黑色，如vec4未设置第四个分量
	m_lightShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b, m_lightColor.a);

	m_targetShader->use();
	m_targetShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b);

	m_targetShader->setInt("material.diffuse", 0); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_targetShader->setInt("material.specular", 1); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_targetShader->setInt("material.emission", 2); // note 放射光分量
	m_targetShader->setFloat("material.shininess", 64.0f);

	// directional light
	m_targetShader->setVec("dirLight.direction", -0.2f, -1.0f, -0.3f);
	m_targetShader->setVec("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	m_targetShader->setVec("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	m_targetShader->setVec("dirLight.specular", 0.5f, 0.5f, 0.5f);

	// point light auto
	m_targetShader->setVec("pointLight.position", m_lightPos);
	m_targetShader->setVec("pointLight.ambient", 0.2f, 0.2f, 0.2f);
	m_targetShader->setVec("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("pointLight.specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setFloat("pointLight.constant", 1.0f);
	m_targetShader->setFloat("pointLight.linear", 0.09f);
	m_targetShader->setFloat("pointLight.quadratic", 0.032f);

	// point light 1
	m_targetShader->setVec("pointLights[0].position", ESCD::pointLightPositions[0]);
	m_targetShader->setVec("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	m_targetShader->setVec("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setFloat("pointLights[0].constant", 1.0f);
	m_targetShader->setFloat("pointLights[0].linear", 0.09f);
	m_targetShader->setFloat("pointLights[0].quadratic", 0.032f);

	// point light 2
	m_targetShader->setVec("pointLights[1].position", ESCD::pointLightPositions[1]);
	m_targetShader->setVec("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	m_targetShader->setVec("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setFloat("pointLights[1].constant", 1.0f);
	m_targetShader->setFloat("pointLights[1].linear", 0.09f);
	m_targetShader->setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	m_targetShader->setVec("pointLights[2].position", ESCD::pointLightPositions[2]);
	m_targetShader->setVec("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	m_targetShader->setVec("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setFloat("pointLights[2].constant", 1.0f);
	m_targetShader->setFloat("pointLights[2].linear", 0.09f);
	m_targetShader->setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	m_targetShader->setVec("pointLights[3].position", ESCD::pointLightPositions[3]);
	m_targetShader->setVec("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	m_targetShader->setVec("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setFloat("pointLights[3].constant", 1.0f);
	m_targetShader->setFloat("pointLights[3].linear", 0.09f);
	m_targetShader->setFloat("pointLights[3].quadratic", 0.032f);
	// spotLight
	m_targetShader->setVec("spotLight.position", Singleton<CameraWrapper>::instance()->cameraPos());
	m_targetShader->setVec("spotLight.direction", Singleton<CameraWrapper>::instance()->cameraFrontPos());
	m_targetShader->setVec("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	m_targetShader->setVec("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("spotLight.specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setFloat("spotLight.constant", 1.0f);
	m_targetShader->setFloat("spotLight.linear", 0.09f);
	m_targetShader->setFloat("spotLight.quadratic", 0.032f);
	m_targetShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(5.5f)));
	m_targetShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	Singleton<CameraWrapper>::instance()->setCursorVisible(false);
}

void MultipleLightSourceExerciceState::draw()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_lightShader->use();
	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置
	m_lightPos.x = 1.0f + sin((float)glfwGetTime()) * 2.0f;// 值域 -3 到 3
	m_lightPos.y = sin((float)glfwGetTime() / 2.0f) * 1.0f;// -1 到 1

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, m_lightPos);
	modelMat = glm::scale(modelMat, glm::vec3(0.2f));
	m_lightShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	glm::mat4 viewMat = Singleton<CameraWrapper>::instance()->lookAtMatrix();
	m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

	// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
	glm::mat4 projectionMat = glm::perspective(glm::radians(Singleton<CameraWrapper>::instance()->fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
	m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

	// 1个自动、4个固定point light 
	m_lightShader->setVec("lightColor", m_lightColor);
	m_lightVAO->bindVAO();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	for (unsigned int i = 0; i < 4; i++) {
		glm::mat4 modelMat;
		modelMat = glm::translate(modelMat, ESCD::pointLightPositions[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.2f));
		m_lightShader->setMatrix("modelMat", glm::value_ptr(modelMat));

		m_lightShader->setVec("lightColor", glm::vec4(ESCD::pointLightColors[i], 1.0f));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	m_targetShader->use();

	auto cameraPos = Singleton<CameraWrapper>::instance()->cameraPos();
	m_targetShader->setVec("viewPos", cameraPos);

	m_targetShader->setVec("pointLight.position", m_lightPos);

	m_targetShader->setVec("spotLight.position", cameraPos);
	m_targetShader->setVec("spotLight.direction", Singleton<CameraWrapper>::instance()->cameraFrontPos());

	m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
	m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

	for (unsigned int i = 0; i < 10; i++) {
		glm::mat4 modelMat;
		modelMat = glm::translate(modelMat, ESCD::cubePositions[i]);
		float angle = 20.0f * i;
		modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_targetShader->setMatrix("modelMat", glm::value_ptr(modelMat));

		m_targetVAO->bindVAO();
		m_targetVAO->bindTexture();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

ModelExerciceState::ModelExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	m_model = std::make_unique<Model>("skin/nanosuit/nanosuit.obj");

	m_lightVAO = Singleton<TriangleVAOFactory>::instance()->createLightVAO();
	m_targetVAO = Singleton<TriangleVAOFactory>::instance()->createLightMapTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = Singleton<ShaderFactory>::instance()->shaderProgram("cast_light_map_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = Singleton<ShaderFactory>::instance()->shaderProgram("mult_target_map_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Light/mult_light_target_shader.fs");
	m_modelShader = Singleton<ShaderFactory>::instance()->shaderProgram("model_target_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Model/model_target_shader.fs");

	m_lightShader->use();
	// 常见问题：着色器中变量类型与设置时不匹配时，片段着色器输出黑色，如vec4未设置第四个分量
	m_lightShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b, m_lightColor.a);

	m_targetShader->use();
	m_targetShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b);

	m_targetShader->setInt("material.diffuse", 0); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_targetShader->setInt("material.specular", 1); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_targetShader->setInt("material.emission", 2); // note 放射光分量
	m_targetShader->setFloat("material.shininess", 64.0f);

	// factory scene
	// directional light
	m_targetShader->setVec("dirLight.direction", -0.2f, -1.0f, -0.3f);
	m_targetShader->setVec("dirLight.ambient", 0.05f, 0.05f, 0.1f);
	m_targetShader->setVec("dirLight.diffuse", 0.2f, 0.2f, 0.7f);
	m_targetShader->setVec("dirLight.specular", 0.7f, 0.7f, 0.7f);

	// point light auto
	m_targetShader->setVec("pointLight.position", m_lightPos);
	m_targetShader->setVec("pointLight.ambient", 0.2f, 0.2f, 0.2f);
	m_targetShader->setVec("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("pointLight.specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setFloat("pointLight.constant", 1.0f);
	m_targetShader->setFloat("pointLight.linear", 0.09f);
	m_targetShader->setFloat("pointLight.quadratic", 0.032f);

	// point light 1
	m_targetShader->setVec("pointLights[0].position", ESCD::pointLightPositions[0]);
	m_targetShader->setVec("pointLights[0].ambient", ESCD::pointLightColors[0].x * 0.1f, ESCD::pointLightColors[0].y * 0.1f, ESCD::pointLightColors[0].z * 0.1f);
	m_targetShader->setVec("pointLights[0].diffuse", ESCD::pointLightColors[0].x, ESCD::pointLightColors[0].y, ESCD::pointLightColors[0].z);
	m_targetShader->setVec("pointLights[0].specular", ESCD::pointLightColors[0].x, ESCD::pointLightColors[0].y, ESCD::pointLightColors[0].z);
	m_targetShader->setFloat("pointLights[0].constant", 1.0f);
	m_targetShader->setFloat("pointLights[0].linear", 0.09f);
	m_targetShader->setFloat("pointLights[0].quadratic", 0.032f);

	// point light 2
	m_targetShader->setVec("pointLights[1].position", ESCD::pointLightPositions[1]);
	m_targetShader->setVec("pointLights[1].ambient", ESCD::pointLightColors[1].x * 0.1f, ESCD::pointLightColors[1].y * 0.1f, ESCD::pointLightColors[1].z * 0.1f);
	m_targetShader->setVec("pointLights[1].diffuse", ESCD::pointLightColors[1].x, ESCD::pointLightColors[1].y, ESCD::pointLightColors[1].z);
	m_targetShader->setVec("pointLights[1].specular", ESCD::pointLightColors[1].x, ESCD::pointLightColors[1].y, ESCD::pointLightColors[1].z);
	m_targetShader->setFloat("pointLights[1].constant", 1.0f);
	m_targetShader->setFloat("pointLights[1].linear", 0.09f);
	m_targetShader->setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	m_targetShader->setVec("pointLights[2].position", ESCD::pointLightPositions[2]);
	m_targetShader->setVec("pointLights[2].ambient", ESCD::pointLightColors[2].x * 0.1f, ESCD::pointLightColors[2].y * 0.1f, ESCD::pointLightColors[2].z * 0.1f);
	m_targetShader->setVec("pointLights[2].diffuse", ESCD::pointLightColors[2].x, ESCD::pointLightColors[2].y, ESCD::pointLightColors[2].z);
	m_targetShader->setVec("pointLights[2].specular", ESCD::pointLightColors[2].x, ESCD::pointLightColors[2].y, ESCD::pointLightColors[2].z);
	m_targetShader->setFloat("pointLights[2].constant", 1.0f);
	m_targetShader->setFloat("pointLights[2].linear", 0.09f);
	m_targetShader->setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	m_targetShader->setVec("pointLights[3].position", ESCD::pointLightPositions[3]);
	m_targetShader->setVec("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	m_targetShader->setVec("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("pointLights[3].ambient", ESCD::pointLightColors[3].x * 0.1f, ESCD::pointLightColors[3].y * 0.1f, ESCD::pointLightColors[3].z * 0.1f);
	m_targetShader->setVec("pointLights[3].diffuse", ESCD::pointLightColors[3].x, ESCD::pointLightColors[3].y, ESCD::pointLightColors[3].z);
	m_targetShader->setVec("pointLights[3].specular", ESCD::pointLightColors[3].x, ESCD::pointLightColors[3].y, ESCD::pointLightColors[3].z);
	m_targetShader->setFloat("pointLights[3].constant", 1.0f);
	m_targetShader->setFloat("pointLights[3].linear", 0.09f);
	m_targetShader->setFloat("pointLights[3].quadratic", 0.032f);
	// spotLight
	m_targetShader->setVec("spotLight.position", Singleton<CameraWrapper>::instance()->cameraPos());
	m_targetShader->setVec("spotLight.direction", Singleton<CameraWrapper>::instance()->cameraFrontPos());
	m_targetShader->setVec("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	m_targetShader->setVec("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("spotLight.specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setFloat("spotLight.constant", 1.0f);
	m_targetShader->setFloat("spotLight.linear", 0.09f);
	m_targetShader->setFloat("spotLight.quadratic", 0.032f);
	m_targetShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(5.5f)));
	m_targetShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


	// 人物模型加载
	m_modelShader->use();

	m_modelShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b);
	m_modelShader->setInt("material.diffuse", 0); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_modelShader->setInt("material.specular", 1); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_modelShader->setInt("material.emission", 2); // note 放射光分量
	m_modelShader->setFloat("material.shininess", 64.0f);

	// directional light
	m_modelShader->setVec("dirLight.direction", -0.2f, -1.0f, -0.3f);
	m_modelShader->setVec("dirLight.ambient", 0.05f, 0.05f, 0.1f);
	m_modelShader->setVec("dirLight.diffuse", 0.2f, 0.2f, 0.7f);
	m_modelShader->setVec("dirLight.specular", 0.7f, 0.7f, 0.7f);

	// point light auto
	m_modelShader->setVec("pointLight.position", m_lightPos);
	m_modelShader->setVec("pointLight.ambient", 0.2f, 0.2f, 0.2f);
	m_modelShader->setVec("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
	m_modelShader->setVec("pointLight.specular", 1.0f, 1.0f, 1.0f);
	m_modelShader->setFloat("pointLight.constant", 1.0f);
	m_modelShader->setFloat("pointLight.linear", 0.09f);
	m_modelShader->setFloat("pointLight.quadratic", 0.032f);

	// point light 1
	m_modelShader->setVec("pointLights[0].position", ESCD::pointLightPositions[0]);
	m_modelShader->setVec("pointLights[0].ambient", ESCD::pointLightColors[0].x * 0.1f, ESCD::pointLightColors[0].y * 0.1f, ESCD::pointLightColors[0].z * 0.1f);
	m_modelShader->setVec("pointLights[0].diffuse", ESCD::pointLightColors[0].x, ESCD::pointLightColors[0].y, ESCD::pointLightColors[0].z);
	m_modelShader->setVec("pointLights[0].specular", ESCD::pointLightColors[0].x, ESCD::pointLightColors[0].y, ESCD::pointLightColors[0].z);
	m_modelShader->setFloat("pointLights[0].constant", 1.0f);
	m_modelShader->setFloat("pointLights[0].linear", 0.09f);
	m_modelShader->setFloat("pointLights[0].quadratic", 0.032f);

	// point light 2
	m_modelShader->setVec("pointLights[1].position", ESCD::pointLightPositions[1]);
	m_modelShader->setVec("pointLights[1].ambient", ESCD::pointLightColors[1].x * 0.1f, ESCD::pointLightColors[1].y * 0.1f, ESCD::pointLightColors[1].z * 0.1f);
	m_modelShader->setVec("pointLights[1].diffuse", ESCD::pointLightColors[1].x, ESCD::pointLightColors[1].y, ESCD::pointLightColors[1].z);
	m_modelShader->setVec("pointLights[1].specular", ESCD::pointLightColors[1].x, ESCD::pointLightColors[1].y, ESCD::pointLightColors[1].z);
	m_modelShader->setFloat("pointLights[1].constant", 1.0f);
	m_modelShader->setFloat("pointLights[1].linear", 0.09f);
	m_modelShader->setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	m_modelShader->setVec("pointLights[2].position", ESCD::pointLightPositions[2]);
	m_modelShader->setVec("pointLights[2].ambient", ESCD::pointLightColors[2].x * 0.1f, ESCD::pointLightColors[2].y * 0.1f, ESCD::pointLightColors[2].z * 0.1f);
	m_modelShader->setVec("pointLights[2].diffuse", ESCD::pointLightColors[2].x, ESCD::pointLightColors[2].y, ESCD::pointLightColors[2].z);
	m_modelShader->setVec("pointLights[2].specular", ESCD::pointLightColors[2].x, ESCD::pointLightColors[2].y, ESCD::pointLightColors[2].z);
	m_modelShader->setFloat("pointLights[2].constant", 1.0f);
	m_modelShader->setFloat("pointLights[2].linear", 0.09f);
	m_modelShader->setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	m_modelShader->setVec("pointLights[3].position", ESCD::pointLightPositions[3]);
	m_modelShader->setVec("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	m_modelShader->setVec("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	m_modelShader->setVec("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	m_modelShader->setVec("pointLights[3].ambient", ESCD::pointLightColors[3].x * 0.1f, ESCD::pointLightColors[3].y * 0.1f, ESCD::pointLightColors[3].z * 0.1f);
	m_modelShader->setVec("pointLights[3].diffuse", ESCD::pointLightColors[3].x, ESCD::pointLightColors[3].y, ESCD::pointLightColors[3].z);
	m_modelShader->setVec("pointLights[3].specular", ESCD::pointLightColors[3].x, ESCD::pointLightColors[3].y, ESCD::pointLightColors[3].z);
	m_modelShader->setFloat("pointLights[3].constant", 1.0f);
	m_modelShader->setFloat("pointLights[3].linear", 0.09f);
	m_modelShader->setFloat("pointLights[3].quadratic", 0.032f);
	// spotLight
	m_modelShader->setVec("spotLight.position", Singleton<CameraWrapper>::instance()->cameraPos());
	m_modelShader->setVec("spotLight.direction", Singleton<CameraWrapper>::instance()->cameraFrontPos());
	m_modelShader->setVec("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	m_modelShader->setVec("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	m_modelShader->setVec("spotLight.specular", 1.0f, 1.0f, 1.0f);
	m_modelShader->setFloat("spotLight.constant", 1.0f);
	m_modelShader->setFloat("spotLight.linear", 0.09f);
	m_modelShader->setFloat("spotLight.quadratic", 0.032f);
	m_modelShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(5.5f)));
	m_modelShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	Singleton<CameraWrapper>::instance()->setCursorVisible(false);
}

void ModelExerciceState::draw()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_lightShader->use();
	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置
	m_lightPos.x = 1.0f + sin((float)glfwGetTime()) * 2.0f;// 值域 -3 到 3
	m_lightPos.y = sin((float)glfwGetTime() / 2.0f) * 1.0f;// -1 到 1

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, m_lightPos);
	modelMat = glm::scale(modelMat, glm::vec3(0.2f));
	m_lightShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	glm::mat4 viewMat = Singleton<CameraWrapper>::instance()->lookAtMatrix();
	m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

	// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
	glm::mat4 projectionMat = glm::perspective(glm::radians(Singleton<CameraWrapper>::instance()->fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
	m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

	// 1个自动、4个固定point light 
	m_lightShader->setVec("lightColor", m_lightColor);
	m_lightVAO->bindVAO();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	for (unsigned int i = 0; i < 4; i++) {
		glm::mat4 modelMat;
		modelMat = glm::translate(modelMat, ESCD::pointLightPositions[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.2f));
		m_lightShader->setMatrix("modelMat", glm::value_ptr(modelMat));
		m_lightShader->setVec("lightColor", glm::vec4(ESCD::pointLightColors[i], 1.0f));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	m_targetShader->use();

	auto cameraPos = Singleton<CameraWrapper>::instance()->cameraPos();
	m_targetShader->setVec("viewPos", cameraPos);
	m_targetShader->setVec("pointLight.position", m_lightPos);
	m_targetShader->setVec("spotLight.position", cameraPos);
	m_targetShader->setVec("spotLight.direction", Singleton<CameraWrapper>::instance()->cameraFrontPos());

	m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
	m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));
	for (unsigned int i = 0; i < 10; i++) {
		glm::mat4 modelMat;
		modelMat = glm::translate(modelMat, ESCD::cubePositions[i]);
		float angle = 20.0f * i;
		modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_targetShader->setMatrix("modelMat", glm::value_ptr(modelMat));

		m_targetVAO->bindVAO();
		m_targetVAO->bindTexture();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	m_modelShader->use();
	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(0.0f, -0.2f, 2.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.2f));
	m_modelShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	m_modelShader->setMatrix("viewMat", glm::value_ptr(viewMat));
	m_modelShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

	m_modelShader->setVec("viewPos", cameraPos);
	m_modelShader->setVec("pointLight.position", m_lightPos);
	m_modelShader->setVec("spotLight.position", cameraPos);
	m_modelShader->setVec("spotLight.direction", Singleton<CameraWrapper>::instance()->cameraFrontPos());

	m_model->draw(m_modelShader);
}

