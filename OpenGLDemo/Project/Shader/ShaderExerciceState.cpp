#include "ShaderExerciceState.h"

#include "ShaderFactory.h"

extern GLFWwindow* m_globalWindow;
extern std::unique_ptr<StateContext> m_stateContext;
extern CameraWrapper m_cameraWrapper;

static std::unique_ptr<ShaderFactory> m_shaderFactory = std::make_unique<ShaderFactory>();

bool AbstractExerciceState::match(const std::list<int>& keyList1st, const std::list<int>& keyList2nd)
{
	if (keyList1st.size() != keyList2nd.size())
		return false;

	for (int key : keyList1st) {
		auto iter = std::find(keyList2nd.begin(), keyList2nd.end(), key);
		if (iter == keyList2nd.end()) {
			return false;
		}
	}

	return true;
}

StateContext::StateContext()
{
	setState(std::make_unique<TextureExerciceState>());
}

void StateContext::setPressKey(int key)
{
	m_presskey = key;
}

void StateContext::setPressKeyList(const std::list<int>& keyList)
{
	m_presskeyList.clear();
	m_presskeyList = keyList;
}

void StateContext::use()
{
	if (!m_state)
		return;

	m_state->use(this);
}

void TextureExerciceStateOther::use(StateContext* context)
{
	int key = context->pressKey();
	auto keyList = context->pressKeyList();

	switch (key) {
	case GLFW_KEY_N:
		context->setState(std::make_unique<TextureExerciceStateMix>());
		break;
	case GLFW_KEY_0:
		context->setState(std::make_unique<TextureExerciceState>());
		break;
	case GLFW_KEY_1:
		context->setState(std::make_unique<TextureExerciceState1>());
		break;
	case GLFW_KEY_4:
		context->setState(std::make_unique<TextureExerciceState4>());
		break;
	case GLFW_KEY_5:
		context->setState(std::make_unique<TransformTextureExerciceState>());
		break;	
	case GLFW_KEY_6:
		context->setState(std::make_unique<TextureExerciceState3D>());
		break; 
	case GLFW_KEY_7:
		context->setState(std::make_unique<TextureExerciceStateCube>());
		break;
	case GLFW_KEY_L:
		if (!keyList.empty()) {
			if (match(keyList, { GLFW_KEY_L, GLFW_KEY_1 })) {
				context->setState(std::make_unique<LightMapExerciceState>());
			}
			else if (match(keyList, { GLFW_KEY_L, GLFW_KEY_2 })) {
				context->setState(std::make_unique<LightSourceExerciceState>());
			}
			else if (match(keyList, { GLFW_KEY_L, GLFW_KEY_3 })) {
				context->setState(std::make_unique<FlashLightSourceExerciceState>());
			}
			else if (match(keyList, { GLFW_KEY_L, GLFW_KEY_4 })) {
				context->setState(std::make_unique<MultipleLightSourceExerciceState>());
			}
			else {
				context->setState(std::make_unique<LightSceneExerciceState>());
			}
		}
		else {
			context->setState(std::make_unique<LightSceneExerciceState>());
		}
		break;
	case GLFW_KEY_M:
		if (!keyList.empty()) {
			context->setState(std::make_unique<ModelExerciceState>());
		}
		else {
			context->setState(std::make_unique<ModelExerciceState>());
		}
		break;
	default:
		context->setPressKey(GLFW_KEY_0);
		context->setState(std::make_unique<TextureExerciceState>());
		break;
	}

	context->use();
}

TextureExerciceState::TextureExerciceState()
{
	m_shader = m_shaderFactory->shaderProgram("texture", "ShaderProgram/Texture/texture_shader.vs", "ShaderProgram/Texture/texture_shader.fs");
}

void TextureExerciceState::use(StateContext* context)
{
	if (context->pressKey() == GLFW_KEY_0) {
		m_shader->use();
	}
	else {
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

TextureExerciceStateMix::TextureExerciceStateMix()
{
	m_shader = m_shaderFactory->shaderProgram("texture_unit", "ShaderProgram/Texture/texture_shader.vs", "ShaderProgram/Texture/texture_unit_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0); // 使用着色器程序后，才能对uniform变量赋值
	m_shader->setInt("texture2", 1);
}

void TextureExerciceStateMix::use(StateContext* context)
{
	if (context->pressKey() == GLFW_KEY_N) {
		m_shader->use();
	}
	else {
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

TextureExerciceState1::TextureExerciceState1()
{
	m_shader = m_shaderFactory->shaderProgram("exercise_1_shader", "ShaderProgram/Texture/texture_shader.vs", "ShaderProgram/Texture/exercise_1_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0); // 使用着色器程序后，才能对uniform变量赋值, 设置着色器中的采样器的位置索引
	m_shader->setInt("texture2", 1);
}

void TextureExerciceState1::use(StateContext* context)
{
	if (context->pressKey() == GLFW_KEY_1) {
		m_shader->use();
	}
	else {
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

TextureExerciceState4::TextureExerciceState4()
{
	m_shader = m_shaderFactory->shaderProgram("exercise_4_shader", "ShaderProgram/Texture/texture_shader.vs", "ShaderProgram/Texture/exercise_4_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0);
	m_shader->setInt("texture2", 1);
}

void TextureExerciceState4::use(StateContext* context)
{
	if (context->pressKey() == GLFW_KEY_4) {
		m_shader->use();
	}
	else {
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

TransformTextureExerciceState::TransformTextureExerciceState()
{
	m_shader = m_shaderFactory->shaderProgram("transfrom_shader", "ShaderProgram/Texture/texture_transform_shader.vs", "ShaderProgram/Texture/texture_unit_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0);
	m_shader->setInt("texture2", 1);

	glm::mat4 transformMatrix;
	transformMatrix = glm::rotate(transformMatrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	transformMatrix = glm::scale(transformMatrix, glm::vec3(0.5, 0.5, 0.5));
	m_shader->setMatrix("transformMatrix", glm::value_ptr(transformMatrix));
}

void TransformTextureExerciceState::use(StateContext* context)
{
	if (context->pressKey() == GLFW_KEY_5) {
		m_shader->use();

		glm::mat4 transfromMat;
		float sec = (float)glfwGetTime(); // glfw运行程序的累加时间，相当于Timer，可重置时间
		transfromMat = glm::translate(transfromMat, glm::vec3(0.5f, -0.5f, 0.0f));// 顺序不一致，效果不一致
		transfromMat = glm::rotate(transfromMat, sec, glm::vec3(0.0f, 0.0f, 1.1f));
		m_shader->setMatrix("transformMatrix", glm::value_ptr(transfromMat));

		if (auto VAO = context->vao()) {
			VAO->bindTexture();
			VAO->bindVAO();
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			transfromMat = glm::mat4(1.0f);
			transfromMat = glm::translate(transfromMat, glm::vec3(-0.5f, 0.5f, 0.0f));

			float scaleValue = abs(sin(sec));
			transfromMat = glm::scale(transfromMat, glm::vec3(scaleValue, scaleValue, scaleValue));
			m_shader->setMatrix("transformMatrix", &transfromMat[0][0]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
	else {
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

TextureExerciceState3D::TextureExerciceState3D()
{
	m_shader = m_shaderFactory->shaderProgram("fake_3d_shader", "ShaderProgram/Texture/texture_3d_shader.vs", "ShaderProgram/Texture/texture_unit_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0);
	m_shader->setInt("texture2", 1);

	glm::mat4 transformMatrix;
	transformMatrix = glm::rotate(transformMatrix, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	transformMatrix = glm::scale(transformMatrix, glm::vec3(0.5, 0.5, 0.5));
	m_shader->setMatrix("transformMatrix", glm::value_ptr(transformMatrix));
}

void TextureExerciceState3D::use(StateContext * context)
{
	if (context->pressKey() == GLFW_KEY_6) {
		m_shader->use();

		if (auto VAO = context->vao()) {
			VAO->bindTexture();
			VAO->bindVAO();

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
	}
	else {
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

TextureExerciceStateCube::TextureExerciceStateCube()
{
	m_shader = m_shaderFactory->shaderProgram("cube_shader", "ShaderProgram/Texture/cube_3d_shader.vs", "ShaderProgram/Texture/texture_unit_shader.fs");
	m_shader->use();
	m_shader->setInt("texture1", 0);
	m_shader->setInt("texture2", 1);
}

void TextureExerciceStateCube::use(StateContext * context)
{
	if (context->pressKey() == GLFW_KEY_7) {
		m_cameraWrapper.setCursorVisible(false);
		m_shader->use();

		if (auto VAO = context->vao()) {
			VAO->bindTexture();
			VAO->bindVAO();
			glm::vec3 cubePositions[] = {
			  glm::vec3(0.0f,  0.0f,  0.0f),
			  glm::vec3(2.0f,  5.0f, -15.0f),
			  glm::vec3(-1.5f, -2.2f, -2.5f),
			  glm::vec3(-3.8f, -2.0f, -12.3f),
			  glm::vec3(2.4f, -0.4f, -3.5f),
			  glm::vec3(-1.7f,  3.0f, -7.5f),
			  glm::vec3(1.3f, -2.0f, -2.5f),
			  glm::vec3(1.5f,  2.0f, -2.5f),
			  glm::vec3(1.5f,  0.2f, -1.5f),
			  glm::vec3(-1.3f,  1.0f, -1.5f)
			};

			/*		glm::mat4 viewMat;
					viewMat = glm::translate(viewMat, glm::vec3(0.0f, -0.0f, -3.0f));
					m_shader->setMatrix("viewMat", glm::value_ptr(viewMat));*/

			for (int i = 0; i < 10; ++i) {
				glm::mat4 modelMat;
				modelMat = glm::translate(modelMat, cubePositions[i]);
				
				if (i == 0 || i % 3 == 0) {
					float sec = (float)glfwGetTime();
					modelMat = glm::rotate(modelMat, sec, glm::vec3(1.0f, 0.0f, 0.0f));
					m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));
				}
				else {
					m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));
				}
				
				/*		float radius = 10.0f;
						float camX = (float)sin(glfwGetTime()) * radius;
						float camZ = (float)cos(glfwGetTime()) * radius;*/
		
				//m_cameraWrapper.processKeyPress();
				//auto viewMat = m_cameraWrapper.lookAtMatrix();
				auto viewMat = m_cameraWrapper.customLookAtMatrix();
				m_shader->setMatrix("viewMat", glm::value_ptr(viewMat)); // 将顶点变换到观察矩阵的坐标空间

				glm::mat4 projectionMat;
				//glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);正射投影矩阵
				// 透视投影矩阵，距离越远w其次分量越大，vertex/w 顶点越小，虚拟远近效果
				// 生成平截头体
				//projectionMat = glm::perspective(glm::radians(45.0f), float(800) / (float)600, 0.1f, 100.0f);
				projectionMat = glm::perspective(glm::radians(m_cameraWrapper.fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
				m_shader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

			
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
	}
	else {
		m_cameraWrapper.setCursorVisible(true);
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

LightSceneExerciceState::LightSceneExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
	, m_targetColor(glm::vec4(1.0f, 0.5f, 0.31f, 1.0f))
{
	auto triangleVAOFactory = std::make_unique<TriangleVAOFactory>();
	m_lightVAO = triangleVAOFactory->createLightVAO();
	m_targetVAO = triangleVAOFactory->createTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = m_shaderFactory->shaderProgram("light_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = m_shaderFactory->shaderProgram("target_shader", "ShaderProgram/Light/target_shader.vs", "ShaderProgram/Light/target_shader.fs");
	
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

	m_cameraWrapper.setCursorVisible(false);
}

void LightSceneExerciceState::use(StateContext * context)
{
	auto& keyList = context->pressKeyList();
	if (match(keyList, { GLFW_KEY_L })) {
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
		glm::mat4 viewMat = m_cameraWrapper.lookAtMatrix();
		m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

		// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
		glm::mat4 projectionMat = glm::perspective(glm::radians(m_cameraWrapper.fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
		m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

		m_lightVAO->bindVAO();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		m_targetShader->use();
		modelMat = glm::mat4(1.0f);
		m_targetShader->setMatrix("modelMat", glm::value_ptr(modelMat));
		m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
		m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));
		auto cameraPos = m_cameraWrapper.cameraPos();
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
	else {
		m_cameraWrapper.setCursorVisible(true);
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

LightMapExerciceState::LightMapExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	auto triangleVAOFactory = std::make_unique<TriangleVAOFactory>();
	m_lightVAO = triangleVAOFactory->createLightVAO();
	m_targetVAO = triangleVAOFactory->createLightMapTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = m_shaderFactory->shaderProgram("texture_light_map_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = m_shaderFactory->shaderProgram("texture_target_map_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Light/texture_map_target_shader.fs");

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

	m_cameraWrapper.setCursorVisible(false);
}

void LightMapExerciceState::use(StateContext* context)
{
	auto& keyList = context->pressKeyList();
	if (match(keyList, { GLFW_KEY_L, GLFW_KEY_1 })) {
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
		glm::mat4 viewMat = m_cameraWrapper.lookAtMatrix();
		m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

		// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
		glm::mat4 projectionMat = glm::perspective(glm::radians(m_cameraWrapper.fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
		m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

		m_lightVAO->bindVAO();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		m_targetShader->use();
		modelMat = glm::mat4(1.0f);
		m_targetShader->setMatrix("modelMat", glm::value_ptr(modelMat));
		m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
		m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));
		auto cameraPos = m_cameraWrapper.cameraPos();
		m_targetShader->setVec("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
		m_targetShader->setVec("lightAttr.position", m_lightPos.x, m_lightPos.y, m_lightPos.z);

		m_targetVAO->bindVAO();
		m_targetVAO->bindTexture();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	else {
		m_cameraWrapper.setCursorVisible(true);
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
};

LightSourceExerciceState::LightSourceExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	auto triangleVAOFactory = std::make_unique<TriangleVAOFactory>();
	m_lightVAO = triangleVAOFactory->createLightVAO();
	m_targetVAO = triangleVAOFactory->createLightMapTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = m_shaderFactory->shaderProgram("cast_light_map_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = m_shaderFactory->shaderProgram("cast_target_map_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Light/map_target_shader.fs");

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

	m_cameraWrapper.setCursorVisible(false);
}

void LightSourceExerciceState::use(StateContext* context)
{
	auto& keyList = context->pressKeyList();
	if (match(keyList, { GLFW_KEY_L, GLFW_KEY_2 })) {
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
		glm::mat4 viewMat = m_cameraWrapper.lookAtMatrix();
		m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

		// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
		glm::mat4 projectionMat = glm::perspective(glm::radians(m_cameraWrapper.fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
		m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

		m_lightVAO->bindVAO();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		m_targetShader->use();
		m_targetShader->setVec("lightAttr.position", m_cameraWrapper.cameraPos());
		m_targetShader->setVec("viewPos", m_cameraWrapper.cameraPos());

		m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
		m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 modelMat;
			modelMat = glm::translate(modelMat, cubePositions[i]);
			float angle = 20.0f * i;
			modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			m_targetShader->setMatrix("modelMat", glm::value_ptr(modelMat));

			m_targetVAO->bindVAO();
			m_targetVAO->bindTexture();
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	else {
		m_cameraWrapper.setCursorVisible(true);
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

FlashLightSourceExerciceState::FlashLightSourceExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	auto triangleVAOFactory = std::make_unique<TriangleVAOFactory>();
	m_lightVAO = triangleVAOFactory->createLightVAO();
	m_targetVAO = triangleVAOFactory->createLightMapTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = m_shaderFactory->shaderProgram("flash_light_light_map_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = m_shaderFactory->shaderProgram("flash_light_target_map_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Light/flash_light_target_shader.fs");

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
	m_targetShader->setVec("lightAttr.position", m_cameraWrapper.cameraPos());
	m_targetShader->setVec("lightAttr.direction", m_cameraWrapper.cameraFrontPos());
	m_targetShader->setFloat("lightAttr.cutoff", glm::cos(glm::radians(5.5f))); // 切光角
	m_targetShader->setFloat("lightAttr.outerCutoff", glm::cos(glm::radians(17.5f))); // 切光角

	m_targetShader->setVec("lightAttr.ambient", 0.1f, 0.1f, 0.1f);
	m_targetShader->setVec("lightAttr.diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("lightAttr.specular", 1.0f, 1.0f, 1.0f);
	// 衰减系数
	m_targetShader->setFloat("lightAttr.constant", 1.0f);
	m_targetShader->setFloat("lightAttr.linear", 0.09f);
	m_targetShader->setFloat("lightAttr.quadratic", 0.032f);

	m_cameraWrapper.setCursorVisible(false);
}

void FlashLightSourceExerciceState::use(StateContext* context)
{
	auto& keyList = context->pressKeyList();
	if (match(keyList, { GLFW_KEY_L, GLFW_KEY_3 })) {
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
		glm::mat4 viewMat = m_cameraWrapper.lookAtMatrix();
		m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

		// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
		glm::mat4 projectionMat = glm::perspective(glm::radians(m_cameraWrapper.fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
		m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

		//m_lightVAO->bindVAO();
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		m_targetShader->use();
		// 聚光手电筒
		m_targetShader->setVec("lightAttr.position", m_cameraWrapper.cameraPos());
		m_targetShader->setVec("lightAttr.direction", m_cameraWrapper.cameraFrontPos());

		m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
		m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));
		m_targetShader->setVec("viewPos", m_cameraWrapper.cameraPos());

		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 modelMat;
			modelMat = glm::translate(modelMat, cubePositions[i]);
			float angle = 20.0f * i;
			modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			m_targetShader->setMatrix("modelMat", glm::value_ptr(modelMat));

			m_targetVAO->bindVAO();
			m_targetVAO->bindTexture();
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	else {
		m_cameraWrapper.setCursorVisible(true);
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

MultipleLightSourceExerciceState::MultipleLightSourceExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	pointLightPositions = new glm::vec3[4]{
		glm::vec3(-2.3f,  0.2f,  0.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	pointLightColors = new glm::vec3[4]{
		glm::vec3(0.2f, 0.2f, 0.6f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.3f),
		glm::vec3(0.4f, 0.4f, 0.4f)
	};

	auto triangleVAOFactory = std::make_unique<TriangleVAOFactory>();
	m_lightVAO = triangleVAOFactory->createLightVAO();
	m_targetVAO = triangleVAOFactory->createLightMapTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = m_shaderFactory->shaderProgram("mult_light_map_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = m_shaderFactory->shaderProgram("mult_target_map_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Light/mult_light_target_shader.fs");

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
	//m_targetShader->setVec("dirLight.direction", -0.2f, -1.0f, -0.3f);
	//m_targetShader->setVec("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	//m_targetShader->setVec("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	//m_targetShader->setVec("dirLight.specular", 0.5f, 0.5f, 0.5f);

	m_targetShader->setVec("dirLight.direction", -0.2f, -1.0f, -0.3f);
	m_targetShader->setVec("dirLight.ambient", 0.05f, 0.05f, 0.1f);
	m_targetShader->setVec("dirLight.diffuse", 0.2f, 0.2f, 0.7);
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
	m_targetShader->setVec("pointLights[0].position", pointLightPositions[0]);
	//m_targetShader->setVec("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	//m_targetShader->setVec("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	//m_targetShader->setVec("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
	m_targetShader->setVec("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	m_targetShader->setVec("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	m_targetShader->setFloat("pointLights[0].constant", 1.0f);
	m_targetShader->setFloat("pointLights[0].linear", 0.09f);
	m_targetShader->setFloat("pointLights[0].quadratic", 0.032f);

	// point light 2
	m_targetShader->setVec("pointLights[1].position", pointLightPositions[1]);
	//m_targetShader->setVec("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	//m_targetShader->setVec("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	//m_targetShader->setVec("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);
	m_targetShader->setVec("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	m_targetShader->setVec("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	m_targetShader->setFloat("pointLights[1].constant", 1.0f);
	m_targetShader->setFloat("pointLights[1].linear", 0.09f);
	m_targetShader->setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	m_targetShader->setVec("pointLights[2].position", pointLightPositions[2]);
	//m_targetShader->setVec("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	//m_targetShader->setVec("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	//m_targetShader->setVec("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);
	m_targetShader->setVec("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	m_targetShader->setVec("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	m_targetShader->setFloat("pointLights[2].constant", 1.0f);
	m_targetShader->setFloat("pointLights[2].linear", 0.09f);
	m_targetShader->setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	m_targetShader->setVec("pointLights[3].position", pointLightPositions[3]);
	m_targetShader->setVec("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	m_targetShader->setVec("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);
	m_targetShader->setVec("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	m_targetShader->setVec("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	m_targetShader->setFloat("pointLights[3].constant", 1.0f);
	m_targetShader->setFloat("pointLights[3].linear", 0.09f);
	m_targetShader->setFloat("pointLights[3].quadratic", 0.032f);
	// spotLight
	m_targetShader->setVec("spotLight.position", m_cameraWrapper.cameraPos());
	m_targetShader->setVec("spotLight.direction", m_cameraWrapper.cameraFrontPos());
	m_targetShader->setVec("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	m_targetShader->setVec("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("spotLight.specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setFloat("spotLight.constant", 1.0f);
	m_targetShader->setFloat("spotLight.linear", 0.09f);
	m_targetShader->setFloat("spotLight.quadratic", 0.032f);
	m_targetShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(5.5f)));
	m_targetShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	m_cameraWrapper.setCursorVisible(false);
}

MultipleLightSourceExerciceState::~MultipleLightSourceExerciceState()
{
	delete[] pointLightPositions;
	delete[] pointLightColors;
}

void MultipleLightSourceExerciceState::use(StateContext* context)
{
	auto& keyList = context->pressKeyList();
	if (match(keyList, { GLFW_KEY_L, GLFW_KEY_4 })) {
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
		glm::mat4 viewMat = m_cameraWrapper.lookAtMatrix();
		m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

		// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
		glm::mat4 projectionMat = glm::perspective(glm::radians(m_cameraWrapper.fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
		m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

		// 1个自动、4个固定point light 
		m_lightShader->setVec("lightColor", m_lightColor);
		m_lightVAO->bindVAO();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	
		for (unsigned int i = 0; i < 4; i++) {
			glm::mat4 modelMat;
			modelMat = glm::translate(modelMat, pointLightPositions[i]);
			modelMat = glm::scale(modelMat, glm::vec3(0.2f));
			m_lightShader->setMatrix("modelMat", glm::value_ptr(modelMat));

			m_lightShader->setVec("lightColor", glm::vec4(pointLightColors[i], 1.0f));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		m_targetShader->use();

		auto cameraPos = m_cameraWrapper.cameraPos();
		m_targetShader->setVec("viewPos", cameraPos);

		m_targetShader->setVec("pointLight.position", m_lightPos);

		m_targetShader->setVec("spotLight.position", cameraPos);
		m_targetShader->setVec("spotLight.direction", m_cameraWrapper.cameraFrontPos());

		m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
		m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 modelMat;
			modelMat = glm::translate(modelMat, cubePositions[i]);
			float angle = 20.0f * i;
			modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			m_targetShader->setMatrix("modelMat", glm::value_ptr(modelMat));

			m_targetVAO->bindVAO();
			m_targetVAO->bindTexture();
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	else {
		m_cameraWrapper.setCursorVisible(true);
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}

glm::vec3 pointLightPositions[] = {
		glm::vec3(-2.3f,  0.2f,  0.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
};

glm::vec3 pointLightColors[] = {
	glm::vec3(0.2f, 0.2f, 0.6f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.3f),
	glm::vec3(0.4f, 0.4f, 0.4f)
};

ModelExerciceState::ModelExerciceState()
	: m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f))
	, m_lightColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	m_model = std::make_unique<Model>("skin/nanosuit/nanosuit.obj");

	auto triangleVAOFactory = std::make_unique<TriangleVAOFactory>();
	m_lightVAO = triangleVAOFactory->createLightVAO();
	m_targetVAO = triangleVAOFactory->createLightMapTargetVAO();

	// 常见问题：选择的了错误的着色器，导致无渲染图像
	m_lightShader = m_shaderFactory->shaderProgram("cast_light_map_shader", "ShaderProgram/Light/light_shader.vs", "ShaderProgram/Light/light_shader.fs");
	m_targetShader = m_shaderFactory->shaderProgram("mult_target_map_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Light/mult_light_target_shader.fs");
	m_modelShader = m_shaderFactory->shaderProgram("model_target_shader", "ShaderProgram/Light/map_target_shader.vs", "ShaderProgram/Model/model_target_shader.fs");

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
	m_targetShader->setVec("dirLight.diffuse", 0.2f, 0.2f, 0.7);
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
	m_targetShader->setVec("pointLights[0].position", pointLightPositions[0]);
	m_targetShader->setVec("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
	m_targetShader->setVec("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	m_targetShader->setVec("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	m_targetShader->setFloat("pointLights[0].constant", 1.0f);
	m_targetShader->setFloat("pointLights[0].linear", 0.09f);
	m_targetShader->setFloat("pointLights[0].quadratic", 0.032f);

	// point light 2
	m_targetShader->setVec("pointLights[1].position", pointLightPositions[1]);
	m_targetShader->setVec("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);
	m_targetShader->setVec("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	m_targetShader->setVec("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	m_targetShader->setFloat("pointLights[1].constant", 1.0f);
	m_targetShader->setFloat("pointLights[1].linear", 0.09f);
	m_targetShader->setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	m_targetShader->setVec("pointLights[2].position", pointLightPositions[2]);
	m_targetShader->setVec("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);
	m_targetShader->setVec("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	m_targetShader->setVec("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	m_targetShader->setFloat("pointLights[2].constant", 1.0f);
	m_targetShader->setFloat("pointLights[2].linear", 0.09f);
	m_targetShader->setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	m_targetShader->setVec("pointLights[3].position", pointLightPositions[3]);
	m_targetShader->setVec("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	m_targetShader->setVec("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	m_targetShader->setVec("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);
	m_targetShader->setVec("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	m_targetShader->setVec("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	m_targetShader->setFloat("pointLights[3].constant", 1.0f);
	m_targetShader->setFloat("pointLights[3].linear", 0.09f);
	m_targetShader->setFloat("pointLights[3].quadratic", 0.032f);
	// spotLight
	m_targetShader->setVec("spotLight.position", m_cameraWrapper.cameraPos());
	m_targetShader->setVec("spotLight.direction", m_cameraWrapper.cameraFrontPos());
	m_targetShader->setVec("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	m_targetShader->setVec("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	m_targetShader->setVec("spotLight.specular", 1.0f, 1.0f, 1.0f);
	m_targetShader->setFloat("spotLight.constant", 1.0f);
	m_targetShader->setFloat("spotLight.linear", 0.09f);
	m_targetShader->setFloat("spotLight.quadratic", 0.032f);
	m_targetShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(5.5f)));
	m_targetShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


	// 人物模型
	m_modelShader->use();

	m_modelShader->setVec("lightColor", m_lightColor.r, m_lightColor.g, m_lightColor.b);
	m_modelShader->setInt("material.diffuse", 0); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_modelShader->setInt("material.specular", 1); // note 变成纹理采样器位置，而不是之前的漫反射光照材质向量
	m_modelShader->setInt("material.emission", 2); // note 放射光分量
	m_modelShader->setFloat("material.shininess", 64.0f);

	// directional light
	m_modelShader->setVec("dirLight.direction", -0.2f, -1.0f, -0.3f);
	m_modelShader->setVec("dirLight.ambient", 0.05f, 0.05f, 0.1f);
	m_modelShader->setVec("dirLight.diffuse", 0.2f, 0.2f, 0.7);
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
	m_modelShader->setVec("pointLights[0].position", pointLightPositions[0]);
	m_modelShader->setVec("pointLights[0].ambient", pointLightColors[0].x * 0.1, pointLightColors[0].y * 0.1, pointLightColors[0].z * 0.1);
	m_modelShader->setVec("pointLights[0].diffuse", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	m_modelShader->setVec("pointLights[0].specular", pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	m_modelShader->setFloat("pointLights[0].constant", 1.0f);
	m_modelShader->setFloat("pointLights[0].linear", 0.09f);
	m_modelShader->setFloat("pointLights[0].quadratic", 0.032f);

	// point light 2
	m_modelShader->setVec("pointLights[1].position", pointLightPositions[1]);
	m_modelShader->setVec("pointLights[1].ambient", pointLightColors[1].x * 0.1, pointLightColors[1].y * 0.1, pointLightColors[1].z * 0.1);
	m_modelShader->setVec("pointLights[1].diffuse", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	m_modelShader->setVec("pointLights[1].specular", pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	m_modelShader->setFloat("pointLights[1].constant", 1.0f);
	m_modelShader->setFloat("pointLights[1].linear", 0.09f);
	m_modelShader->setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	m_modelShader->setVec("pointLights[2].position", pointLightPositions[2]);
	m_modelShader->setVec("pointLights[2].ambient", pointLightColors[2].x * 0.1, pointLightColors[2].y * 0.1, pointLightColors[2].z * 0.1);
	m_modelShader->setVec("pointLights[2].diffuse", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	m_modelShader->setVec("pointLights[2].specular", pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	m_modelShader->setFloat("pointLights[2].constant", 1.0f);
	m_modelShader->setFloat("pointLights[2].linear", 0.09f);
	m_modelShader->setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	m_modelShader->setVec("pointLights[3].position", pointLightPositions[3]);
	m_modelShader->setVec("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	m_modelShader->setVec("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	m_modelShader->setVec("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	m_modelShader->setVec("pointLights[3].ambient", pointLightColors[3].x * 0.1, pointLightColors[3].y * 0.1, pointLightColors[3].z * 0.1);
	m_modelShader->setVec("pointLights[3].diffuse", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	m_modelShader->setVec("pointLights[3].specular", pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	m_modelShader->setFloat("pointLights[3].constant", 1.0f);
	m_modelShader->setFloat("pointLights[3].linear", 0.09f);
	m_modelShader->setFloat("pointLights[3].quadratic", 0.032f);
	// spotLight
	m_modelShader->setVec("spotLight.position", m_cameraWrapper.cameraPos());
	m_modelShader->setVec("spotLight.direction", m_cameraWrapper.cameraFrontPos());
	m_modelShader->setVec("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	m_modelShader->setVec("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	m_modelShader->setVec("spotLight.specular", 1.0f, 1.0f, 1.0f);
	m_modelShader->setFloat("spotLight.constant", 1.0f);
	m_modelShader->setFloat("spotLight.linear", 0.09f);
	m_modelShader->setFloat("spotLight.quadratic", 0.032f);
	m_modelShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(5.5f)));
	m_modelShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	m_cameraWrapper.setCursorVisible(false);
}

void ModelExerciceState::use(StateContext* context)
{
	auto& keyList = context->pressKeyList();
	if (match(keyList, { GLFW_KEY_M })) {
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
		glm::mat4 viewMat = m_cameraWrapper.lookAtMatrix();
		m_lightShader->setMatrix("viewMat", glm::value_ptr(viewMat));

		// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
		glm::mat4 projectionMat = glm::perspective(glm::radians(m_cameraWrapper.fieldOfView()), float(800) / (float)600, 0.1f, 100.0f);
		m_lightShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));

		// 1个自动、4个固定point light 
		m_lightShader->setVec("lightColor", m_lightColor);
		m_lightVAO->bindVAO();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		for (unsigned int i = 0; i < 4; i++) {
			glm::mat4 modelMat;
			modelMat = glm::translate(modelMat, pointLightPositions[i]);
			modelMat = glm::scale(modelMat, glm::vec3(0.2f));
			m_lightShader->setMatrix("modelMat", glm::value_ptr(modelMat));
			m_lightShader->setVec("lightColor", glm::vec4(pointLightColors[i], 1.0f));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		m_targetShader->use();

		auto cameraPos = m_cameraWrapper.cameraPos();
		m_targetShader->setVec("viewPos", cameraPos);
		m_targetShader->setVec("pointLight.position", m_lightPos);
		m_targetShader->setVec("spotLight.position", cameraPos);
		m_targetShader->setVec("spotLight.direction", m_cameraWrapper.cameraFrontPos());

		m_targetShader->setMatrix("viewMat", glm::value_ptr(viewMat));
		m_targetShader->setMatrix("projectionMat", glm::value_ptr(projectionMat));
		for (unsigned int i = 0; i < 10; i++) {
			glm::mat4 modelMat;
			modelMat = glm::translate(modelMat, cubePositions[i]);
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
		m_modelShader->setVec("spotLight.direction", m_cameraWrapper.cameraFrontPos());

		m_model->draw(m_modelShader);
	}
	else {
		m_cameraWrapper.setCursorVisible(true);
		context->setState(std::make_unique<TextureExerciceStateOther>());
		context->use();
	}
}
