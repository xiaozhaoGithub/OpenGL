#include "AdvancedExerciceState.h"

#include <vector>

#include "Singleton.h"
#include "CommonDataDef.h"
#include "TextureHelper.h"

namespace UCDD = UiCommonDataDef;

AdvancedExerciceState::AdvancedExerciceState()
{
	m_controlParam = std::make_shared<ControlParam>();
	m_controlParam->isPostProcess = false;
	m_controlParam->isMsFramebuffer = true;

	m_cubeVAO = Singleton<TriangleVAOFactory>::instance()->createAdvancedTargetVAO();
	m_planeVAO = Singleton<TriangleVAOFactory>::instance()->createFloorVAO("skin/textures/metal.png");
	m_vegetationVAO = Singleton<TriangleVAOFactory>::instance()->createVegetationVAO();
	m_windowVAO = Singleton<TriangleVAOFactory>::instance()->createWindowVAO();
	m_quadVAO = Singleton<TriangleVAOFactory>::instance()->createQuadVAO();
	m_postProcessCubeVAO = Singleton<TriangleVAOFactory>::instance()->createPostProcessVAO();
	m_skyboxVAO = Singleton<TriangleVAOFactory>::instance()->createSkyboxVAO();
	m_reflectedCubeVAO = Singleton<TriangleVAOFactory>::instance()->createRelectedCubeVAO();
	m_nanosuitModel = std::make_unique<Model>("skin/nanosuit_reflection/nanosuit.obj");
	m_pointVAO = Singleton<TriangleVAOFactory>::instance()->createPointVAO();
	m_pointsVAO = Singleton<TriangleVAOFactory>::instance()->createPointsVAO();
	m_instanceArrayVAO = Singleton<TriangleVAOFactory>::instance()->createInstancedVAO();
	m_planetModel = std::make_unique<Model>("skin/Model/planet/planet.obj");
	m_rockModel = std::make_unique<Model>("skin/Model/rock/rock.obj");

	m_sceneFramebuffer = FramebufferFactory::createFramebuffer();
	m_muiltSampleSceneFramebuffer = FramebufferFactory::createFramebuffer(4);
	m_mediatorSceneFramebuffer = FramebufferFactory::createFramebuffer();

	m_depthTestShader = Singleton<ShaderFactory>::instance()->shaderProgram("depth_test_shader", "ShaderProgram/Advanced/texture_shader.vs", "ShaderProgram/Advanced/depth_test_shader.fs");
	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("cube_texture_shader", "ShaderProgram/Advanced/texture_shader.vs", "ShaderProgram/Advanced/texture_shader.fs");
	m_shader->use();
	m_shader->setInt("sampler1", 0);
	m_singleColorShader = Singleton<ShaderFactory>::instance()->shaderProgram("single_color_shader", "ShaderProgram/Advanced/texture_shader.vs", "ShaderProgram/Advanced/single_color_shader.fs");
	m_transparentShader = Singleton<ShaderFactory>::instance()->shaderProgram("transparent_texture_shader", "ShaderProgram/Advanced/texture_shader.vs", "ShaderProgram/Advanced/transparent_texture_shader.fs");
	m_transparentShader->use();
	m_transparentShader->setInt("sampler1", 0);

	m_screenShader = Singleton<ShaderFactory>::instance()->shaderProgram("screen_texture_shader", "ShaderProgram/Advanced/screen_texture_shader.vs", "ShaderProgram/Advanced/screen_texture_shader.fs");
	m_screenShader->use();
	m_screenShader->setInt("sampler1", 0);

	m_cubeMapShader = Singleton<ShaderFactory>::instance()->shaderProgram("cube_map_shader", "ShaderProgram/Advanced/cube_map_shader.vs", "ShaderProgram/Advanced/cube_map_shader.fs");
	m_cubeMapShader->use();
	m_cubeMapShader->setInt("sampler1", 0);

	m_reflectedCubeShader = Singleton<ShaderFactory>::instance()->shaderProgram("reflected_cube_shader", "ShaderProgram/Advanced/reflected_cube_shader.vs", "ShaderProgram/Advanced/reflected_cube_shader.fs");
	setSampler(m_reflectedCubeShader);

	m_refractShader = Singleton<ShaderFactory>::instance()->shaderProgram("refract_shader", "ShaderProgram/Advanced/reflected_cube_shader.vs", "ShaderProgram/Advanced/refract_shader.fs");
	setSampler(m_refractShader);

	m_reflectMapShader = Singleton<ShaderFactory>::instance()->shaderProgram("reflect_map", "ShaderProgram/Advanced/reflect_map.vs", "ShaderProgram/Advanced/reflect_map.fs");
	m_reflectMapShader->use();
	m_reflectMapShader->setInt("material.texture_cube_map1", 3);

	m_pointsShader = Singleton<ShaderFactory>::instance()->shaderProgram("points_shader", "ShaderProgram/Advanced/points_shader.vs", "ShaderProgram/Advanced/single_color_shader.fs");
	m_pointsShader->use();
	m_pointsShader->setInt("material.texture_cube_map1", 3);

	m_fragCoordShader = Singleton<ShaderFactory>::instance()->shaderProgram("frag_coord_shader", "ShaderProgram/Advanced/frag_coord_shader.vs", "ShaderProgram/Advanced/frag_coord_shader.fs");
	m_fragCoordShader->setUniformBlockBinding("Matrices", 0);
	setSampler(m_fragCoordShader);

	m_linesShader = Singleton<ShaderFactory>::instance()->shaderProgram("geometry_lines_shader", "ShaderProgram/Advanced/geometry_lines_shader.vs", "ShaderProgram/Advanced/geometry_lines_shader.gs", "ShaderProgram/Advanced/single_color_shader.fs");
	m_houseShader = Singleton<ShaderFactory>::instance()->shaderProgram("geometry_house_shader", "ShaderProgram/Advanced/geometry_house_shader.vs", "ShaderProgram/Advanced/geometry_house_shader.gs", "ShaderProgram/Advanced/geometry_house_shader.fs");
	m_explodeShader = Singleton<ShaderFactory>::instance()->shaderProgram("explode_shader", "ShaderProgram/Advanced/explode_shader.vs", "ShaderProgram/Advanced/explode_shader.gs", "ShaderProgram/Advanced/explode_shader.fs");
	m_explodeShader->use();
	m_explodeShader->setInt("material.texture_cube_map1", 3);

	m_normalVisibleShader = Singleton<ShaderFactory>::instance()->shaderProgram("normal_visible_shader", "ShaderProgram/Advanced/normal_visible_shader.vs", "ShaderProgram/Advanced/normal_visible_shader.gs", "ShaderProgram/Advanced/single_color_shader.fs");
	m_instanceArrayShader = Singleton<ShaderFactory>::instance()->shaderProgram("instance_array_shader", "ShaderProgram/Advanced/instance_array_shader.vs", "ShaderProgram/Advanced/instance_array_shader.fs");
	
	m_planetShader = Singleton<ShaderFactory>::instance()->shaderProgram("planet_shader", "ShaderProgram/Advanced/planet_shader.vs", "ShaderProgram/Advanced/normal_model.fs");
	m_planetByInstanceShader = Singleton<ShaderFactory>::instance()->shaderProgram("planet_instance_shader", "ShaderProgram/Advanced/planet_instance_shader.vs", "ShaderProgram/Advanced/normal_model.fs");
	//glm::inverse();
	//glm::transpose();

	m_vegetationPos.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	m_vegetationPos.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	m_vegetationPos.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	m_vegetationPos.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	m_vegetationPos.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

	m_windowPos.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	m_windowPos.push_back(glm::vec3(0.5f, 0.0f, 0.0f));
	m_windowPos.push_back(glm::vec3(-0.7f, 0.0f, 1.0f));

	std::vector<std::string> faces = {
		"skin/textures/skybox/right.jpg",
		"skin/textures/skybox/left.jpg",
		"skin/textures/skybox/top.jpg",
		"skin/textures/skybox/bottom.jpg",
		"skin/textures/skybox/front.jpg",
		"skin/textures/skybox/back.jpg"
	};
	m_skyboxTexId = TextureHelper::loadCubemap(faces);

	initTransformMatUniformBlock();
	initRockModelMat();

	// 必须开启缓冲测试，才能使用相关功能
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_BLEND);
	glEnable(GL_PROGRAM_POINT_SIZE); // 顶点着色器中修改点大小，默认是关闭的
}

AdvancedExerciceState::~AdvancedExerciceState()
{
	glDeleteTextures(1, &m_skyboxTexId);

	delete[] m_rockModelMat;
}

void AdvancedExerciceState::draw()
{
	if (m_controlParam->isPostProcess) {
		if (m_controlParam->isMsFramebuffer) {
			m_muiltSampleSceneFramebuffer->bindFramebuffer();
		}
		else {
			m_sceneFramebuffer->bindFramebuffer();
		}
	}

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	auto cameraWrapper = Singleton<CameraWrapper>::instance();
	cameraWrapper->setCursorVisible(false);

	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	m_viewMat = cameraWrapper->lookAtMatrix();

	// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
	m_projectionMat = glm::perspective(glm::radians(cameraWrapper->fieldOfView()), float(UCDD::kViewportWidth) / UCDD::kViewportHeight, 0.1f, 1000.0f);
	
	m_shader->use();

	glBindBuffer(GL_UNIFORM_BUFFER, m_transformMatUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_viewMat));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_projectionMat));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	drawDepthTest();
	drawFloor();
	drawCube();
	drawVegetation();
	drawWindow();
	drawPostProcessCube();
	drawReflectedCube();
	drawModel();
	drawReflectMapModel();
	drawSkybox();
	drawPoints();
	drawFragCoordCube();
	drawGeometryLines();
	drawGeometryTriangles();
	drawExplodeModel();
	drawNormalVisibleModel();
	drawInstanceByArray();
	//drawPlanetaryBeltByUniform();
	drawPlanetaryBeltByInstance();

	if (m_controlParam->isPostProcess) {
		if (m_controlParam->isMsFramebuffer) {
			m_muiltSampleSceneFramebuffer->blitFramebuffer(m_mediatorSceneFramebuffer->id());
			drawFramebuffer(m_mediatorSceneFramebuffer);
		}
		else {
			drawFramebuffer(m_sceneFramebuffer);
		}
	}
}

void AdvancedExerciceState::setControlParam(std::shared_ptr<ControlParam> controlParam)
{
	m_controlParam = controlParam;
}

void AdvancedExerciceState::drawDepthTest()
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(-4.0f, 0.0f, -1.0f));

	m_depthTestShader->use();
	m_depthTestShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	m_cubeVAO->bindVAO();
	m_cubeVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void AdvancedExerciceState::drawFloor()
{
	glStencilMask(0x00);
	m_shader->use();
	glm::mat4 modelMat = glm::mat4(1.0f);
	m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));

	m_planeVAO->bindVAO();
	m_planeVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void AdvancedExerciceState::drawCube()
{
	// 面剔除，前提: 顶点数据默认以逆时针方向定义一个三角形图元
	// 优化后、提升超过50%的效率
	glEnable(GL_CULL_FACE); 
	glFrontFace(GL_CCW);// default
	glCullFace(GL_BACK); // default

	m_singleColorShader->use();

	// cube 1
	glStencilFunc(GL_ALWAYS, 1, 0xff); // 所有的片段都应该更新模板缓冲
	glStencilMask(0xff); // 启用模板缓冲写入

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(-1.0f, 0.0f, -1.0f));
	m_shader->use();
	m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));

	m_cubeVAO->bindVAO();
	m_cubeVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// border 1
	//glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	glStencilMask(0x00);

	modelMat = glm::scale(modelMat, glm::vec3(1.1f));
	m_singleColorShader->use();
	m_singleColorShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// cube 2
	//glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilMask(0xff);

	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(2.0f, 0.0f, 0.0f));
	m_shader->use();
	m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// border 2
	//glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	glStencilMask(0x00);

	modelMat = glm::scale(modelMat, glm::vec3(1.1f));
	m_singleColorShader->use();
	m_singleColorShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// reset
	//glEnable(GL_DEPTH_TEST);
	glStencilMask(0xff);
	glStencilFunc(GL_ALWAYS, 0, 0xff);
	glDisable(GL_CULL_FACE); // 其他不需要进行面剔除
}

void AdvancedExerciceState::drawVegetation()
{
	// 片段丢弃无法实现渲染半透明的图像
	m_transparentShader->use();

	m_vegetationVAO->bindTexture();
	m_vegetationVAO->bindVAO();

	glm::mat4 modelMat = glm::mat4(1.0f);
	for (unsigned int i = 0; i < m_vegetationPos.size(); i++) {
		modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, m_vegetationPos[i]);
		m_transparentShader->setMatrix("modelMat", glm::value_ptr(modelMat));

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void AdvancedExerciceState::drawWindow()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);// 这样只能让最终的alpha分量被源颜色向量的alpha值所影响到
	
	m_shader->use();

	m_windowVAO->bindTexture();
	m_windowVAO->bindVAO();

	// 根据距离排序, 先绘制距离观察坐标原点较远的blend物体, 深度测试不能识别透明内容, 防止片段被丢弃, 出现窗户中看不到另一个窗户的现象
	// 这种方法对我们这个场景能够正常工作，但它并没有考虑旋转、缩放或其它变换，奇怪形状的物体需要一个不同的计量，而不是仅仅一个位置向量
	// 更高级的技术还有次序无关透明度
	std::map<float, glm::vec3> windowDistancePosMap;
	for (const auto& pos : m_windowPos) {
		windowDistancePosMap[glm::length(Singleton<CameraWrapper>::instance()->cameraPos() - pos)] = pos;
	}

	glm::mat4 modelMat = glm::mat4(1.0f);
	for (auto iter = windowDistancePosMap.rbegin(); iter != windowDistancePosMap.rend(); ++iter) {
		modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, iter->second);
		m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void AdvancedExerciceState::drawPostProcessCube()
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(4.0f, 0.0f, -1.0f));

	m_shader->use();
	m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));

	m_postProcessCubeVAO->bindVAO();
	m_postProcessCubeVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void AdvancedExerciceState::drawReflectedCube()
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(4.0f, 0.0f, 3.0f));

	m_reflectedCubeShader->use();
	m_reflectedCubeShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	m_reflectedCubeShader->setVec("cameraPos", Singleton<CameraWrapper>::instance()->cameraPos());

	m_reflectedCubeVAO->bindVAO();
	m_reflectedCubeVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void AdvancedExerciceState::drawModel()
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(0.0f, -0.49f, 2.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.05f));

	m_refractShader->use();
	m_refractShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	m_nanosuitModel->draw(m_refractShader);
}

void AdvancedExerciceState::drawReflectMapModel()
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(1.0f, -0.49f, 2.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.05f));

	m_reflectMapShader->use();
	m_reflectMapShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	m_reflectMapShader->setVec("cameraPos", Singleton<CameraWrapper>::instance()->cameraPos());
	m_reflectMapShader->setVec("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_reflectMapShader->setInt("material.diffuse", 0); 
	m_reflectMapShader->setInt("material.specular", 1); 
	m_reflectMapShader->setInt("material.emission", 2); 
	m_reflectMapShader->setFloat("material.shininess", 64.0f);

	// directional light
	m_reflectMapShader->setVec("dirLight.direction", -0.2f, -1.0f, -0.3f);
	m_reflectMapShader->setVec("dirLight.ambient", 1.0f, 1.0f, 1.0f);
	m_reflectMapShader->setVec("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
	m_reflectMapShader->setVec("dirLight.specular", 1.0f, 1.0f, 1.0f);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexId);

	m_nanosuitModel->draw(m_reflectMapShader);
}

void AdvancedExerciceState::drawSkybox()
{
	// 放到最后渲染skybox，减少skybox片段着色器执行次数，以获得轻微的性能提升。
	glDepthFunc(GL_LEQUAL);

	m_cubeMapShader->use();

	auto viewMat = glm::mat4(glm::mat3(m_viewMat)); // 移除观察矩阵的平移参数， 产生空间无限大的效果
	m_cubeMapShader->setMatrix("viewMat", glm::value_ptr(viewMat));
	m_cubeMapShader->setMatrix("projectionMat", glm::value_ptr(m_projectionMat));

	m_skyboxVAO->bindVAO();
	m_skyboxVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);
}

void AdvancedExerciceState::drawPoints()
{
	m_pointsShader->use();

	m_pointVAO->bindVAO();

	for (int i = 0; i < 7; ++i) {
		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, glm::vec3(-0.1 * i, 0.1 * i, 2.0f));
		m_pointsShader->setMatrix("modelMat", glm::value_ptr(modelMat));

		//glPointSize(30);
		glDrawArrays(GL_POINTS, 0, 1); // GL_POINTS 而不是 GL_POINT
	}
}

void AdvancedExerciceState::drawFragCoordCube()
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(-4.0f, 0.0f, -4.0f));

	m_fragCoordShader->use();
	m_fragCoordShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	m_cubeVAO->bindVAO();
	m_cubeVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void AdvancedExerciceState::drawGeometryLines()
{
	m_linesShader->use();

	m_pointVAO->bindVAO();
	float step = 0.3f;
	for (int i = 0; i < 4; ++i) {
		glm::mat4 modelMat = glm::mat4(1.0f);
		if (i % 2 == 0) {
			modelMat = glm::translate(modelMat, glm::vec3(-4.0f + i * step, 0.0f, 3.0f));
		}
		else {
			float xOffset = (i - 1) * step;
			int yOffset = i - 2;
			if (yOffset < 0) {
				yOffset = 1;
			}
			modelMat = glm::translate(modelMat, glm::vec3(-4.0f + xOffset, 0.0f + yOffset * step, 3.0f));
		}
		m_pointsShader->setMatrix("modelMat", glm::value_ptr(modelMat));
		glDrawArrays(GL_POINTS, 0, 1);
	}
}

void AdvancedExerciceState::drawGeometryTriangles()
{
	m_houseShader->use();

	glm::mat4 modelMat = glm::mat4(1.0f);
	m_houseShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	m_pointsVAO->bindVAO();
	glDrawArrays(GL_POINTS, 0, 4);
}

void AdvancedExerciceState::drawExplodeModel()
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(2.0f, -0.49f, 2.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.05f));

	m_explodeShader->use();
	m_explodeShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	m_explodeShader->setFloat("time", (float)glfwGetTime());

	m_explodeShader->setVec("cameraPos", Singleton<CameraWrapper>::instance()->cameraPos());
	m_explodeShader->setVec("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_explodeShader->setInt("material.diffuse", 0);
	m_explodeShader->setInt("material.specular", 1);
	m_explodeShader->setInt("material.emission", 2);
	m_explodeShader->setFloat("material.shininess", 64.0f);

	// directional light
	m_explodeShader->setVec("dirLight.direction", -0.2f, -1.0f, -0.3f);
	m_explodeShader->setVec("dirLight.ambient", 1.0f, 1.0f, 1.0f);
	m_explodeShader->setVec("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
	m_explodeShader->setVec("dirLight.specular", 1.0f, 1.0f, 1.0f);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexId);

	m_nanosuitModel->draw(m_explodeShader);
}

void AdvancedExerciceState::drawNormalVisibleModel()
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(3.0f, -0.49f, 2.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.05f));

	m_reflectMapShader->use();
	m_reflectMapShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	m_reflectMapShader->setVec("cameraPos", Singleton<CameraWrapper>::instance()->cameraPos());	m_reflectMapShader->setVec("cameraPos", Singleton<CameraWrapper>::instance()->cameraPos());
	m_reflectMapShader->setVec("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_reflectMapShader->setInt("material.diffuse", 0);
	m_reflectMapShader->setInt("material.specular", 1);
	m_reflectMapShader->setInt("material.emission", 2);
	m_reflectMapShader->setFloat("material.shininess", 64.0f);

	// directional light
	m_reflectMapShader->setVec("dirLight.direction", -0.2f, -1.0f, -0.3f);
	m_reflectMapShader->setVec("dirLight.ambient", 1.0f, 1.0f, 1.0f);
	m_reflectMapShader->setVec("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
	m_reflectMapShader->setVec("dirLight.specular", 1.0f, 1.0f, 1.0f);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexId);
	m_nanosuitModel->draw(m_refractShader);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	m_normalVisibleShader->use();
	m_normalVisibleShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	m_normalVisibleShader->setVec("cameraPos", Singleton<CameraWrapper>::instance()->cameraPos());

	m_nanosuitModel->draw(m_normalVisibleShader);
}

void AdvancedExerciceState::drawInstanceByArray()
{
	m_instanceArrayShader->use();

	m_instanceArrayVAO->bindVAO();

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(-4.0f, 0.5f, -1.0f));
	m_instanceArrayShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
}

void AdvancedExerciceState::drawPlanetaryBeltByUniform()
{
	// planet
	m_planetShader->use();
	m_planetShader->setVec("cameraPos", Singleton<CameraWrapper>::instance()->cameraPos());

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(5.0f, 5.0f, -5.0f));
	modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));
	m_planetShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	m_planetModel->draw(m_planetShader);

	// rock
	for (unsigned int i = 0; i < m_mountOfRock; i++) {
		m_planetShader->setMatrix("modelMat", glm::value_ptr(m_rockModelMat[i]));
		m_rockModel->draw(m_planetShader);
	}
}

void AdvancedExerciceState::drawPlanetaryBeltByInstance()
{	// planet
	m_planetShader->use();
	m_planetShader->setVec("cameraPos", Singleton<CameraWrapper>::instance()->cameraPos());

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(5.0f, 5.0f, -5.0f));
	modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));
	m_planetShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	m_planetModel->draw(m_planetShader);

	// rock
	m_planetByInstanceShader->use();
	m_planetByInstanceShader->setVec("cameraPos", Singleton<CameraWrapper>::instance()->cameraPos());
	m_rockModel->draw(m_planetByInstanceShader, m_mountOfRock);
}

void AdvancedExerciceState::setSampler(std::shared_ptr<AbstractShader> shader)
{
	shader->use();
	shader->setInt("sampler1", 0);
}

void AdvancedExerciceState::initRockModelMat()
{
	srand((unsigned int)time(NULL));

	m_mountOfRock = 1000;
	m_rockModelMat = new glm::mat4[m_mountOfRock];

	float offset = 2.5f;
	float radius = 50.0f;
	for (unsigned int i = 0; i < m_mountOfRock; i++) {
		glm::mat4 modelMat = glm::mat4(1.0f);
		float angle = (float)i / (float)m_mountOfRock * 360.0f;

		float randomValue = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + randomValue;

		randomValue = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = randomValue + 0.4f;

		randomValue = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + randomValue;

		modelMat = glm::translate(modelMat, glm::vec3(x, y, z));

		float scale = (rand() % 20) / 100.0f + 0.05f;
		modelMat = glm::scale(modelMat, glm::vec3(scale));

		float rotAngle = (float)(rand() % 360);
		modelMat = glm::rotate(modelMat, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		m_rockModelMat[i] = modelMat;
	}

	m_rockModel->setRuleCb(std::bind(&AdvancedExerciceState::rockModelRuleCb, this));
}

void AdvancedExerciceState::rockModelRuleCb()
{
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m_mountOfRock, &m_rockModelMat[0], GL_STATIC_DRAW);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 2));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 3));

	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
}

void AdvancedExerciceState::drawFramebuffer(std::shared_ptr<Framebuffer> framebuffer)
{
	// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_screenShader->use();

	m_quadVAO->bindVAO();
	// 常见问题：绑定纹理时，误以为纹理Id保存在VAO中，应该绑定帧缓冲中的纹理对象渲染
	framebuffer->bindTexture(); // use the color attachment texture as the texture of the quad plane

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
}

