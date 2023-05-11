#include "AdvancedExerciceState.h"

#include "Singleton.h"
#include "CommonDataDef.h"

#include <vector>

namespace UCDD = UiCommonDataDef;

AdvancedExerciceState::AdvancedExerciceState()
{
	m_cubeVAO = Singleton<TriangleVAOFactory>::instance()->createAdvancedTargetVAO();
	m_planeVAO = Singleton<TriangleVAOFactory>::instance()->createFloorVAO();
	m_vegetationVAO = Singleton<TriangleVAOFactory>::instance()->createVegetationVAO();
	m_windowVAO = Singleton<TriangleVAOFactory>::instance()->createWindowVAO();
	m_quadVAO = Singleton<TriangleVAOFactory>::instance()->createQuadVAO();
	m_postProcessCubeVAO = Singleton<TriangleVAOFactory>::instance()->creatTextureVAO();
	m_postProcessCubeShader = Singleton<ShaderFactory>::instance()->shaderProgram("texture", "ShaderProgram/Texture/texture_shader.vs", "ShaderProgram/Texture/texture_shader.fs");

	m_sceneFramebuffer = FramebufferFactory::createFramebuffer();

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


	m_vegetationPos.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	m_vegetationPos.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	m_vegetationPos.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	m_vegetationPos.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	m_vegetationPos.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

	m_windowPos.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	m_windowPos.push_back(glm::vec3(0.5f, 0.0f, 0.0f));
	m_windowPos.push_back(glm::vec3(-0.7f, 0.0f, 1.0f));

	// 必须开启缓冲测试，才能使用相关功能
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_BLEND);
}

void AdvancedExerciceState::draw()
{
	m_sceneFramebuffer->bindFramebuffer(); 

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	auto cameraWrapper = Singleton<CameraWrapper>::instance();
	cameraWrapper->setCursorVisible(false);

	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置

	// 观察矩阵主要将顶点变换到观察矩阵的坐标空间
	m_viewMat = cameraWrapper->lookAtMatrix();

	// 投影矩阵, 裁剪可见坐标，并设置透视效果，远处顶点根据齐次分量越大，顶点越小
	m_projectionMat = glm::perspective(glm::radians(cameraWrapper->fieldOfView()), float(UCDD::kViewportWidth) / UCDD::kViewportHeight, 0.1f, 100.0f);
	
	m_shader->use();
	m_shader->setMatrix("viewMat", glm::value_ptr(m_viewMat));
	m_shader->setMatrix("projectionMat", glm::value_ptr(m_projectionMat));

	drawDepthTest();
	drawFloor();
	drawCube();
	drawVegetation();
	drawWindow();
	drawPostProcessCube();
	drawFramebuffer();
}

void AdvancedExerciceState::drawDepthTest()
{
	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(-4.0f, 0.0f, -1.0f));

	m_depthTestShader->use();
	m_depthTestShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	m_depthTestShader->setMatrix("viewMat", glm::value_ptr(m_viewMat));
	m_depthTestShader->setMatrix("projectionMat", glm::value_ptr(m_projectionMat));

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
	m_singleColorShader->setMatrix("viewMat", glm::value_ptr(m_viewMat));
	m_singleColorShader->setMatrix("projectionMat", glm::value_ptr(m_projectionMat));

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
	glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	glStencilMask(0x00);

	modelMat = glm::scale(modelMat, glm::vec3(1.1f));
	m_singleColorShader->use();
	m_singleColorShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// cube 2
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilMask(0xff);

	modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(2.0f, 0.0f, 0.0f));
	m_shader->use();
	m_shader->setMatrix("modelMat", glm::value_ptr(modelMat));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// border 2
	glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	glStencilMask(0x00);

	modelMat = glm::scale(modelMat, glm::vec3(1.1f));
	m_singleColorShader->use();
	m_singleColorShader->setMatrix("modelMat", glm::value_ptr(modelMat));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// reset
	glEnable(GL_DEPTH_TEST);
	glStencilMask(0xff);
	glStencilFunc(GL_ALWAYS, 0, 0xff);
	glDisable(GL_CULL_FACE); // 其他不需要进行面剔除
}

void AdvancedExerciceState::drawVegetation()
{
	// 片段丢弃无法实现渲染半透明的图像
	m_transparentShader->use();
	m_transparentShader->setMatrix("viewMat", glm::value_ptr(m_viewMat));
	m_transparentShader->setMatrix("projectionMat", glm::value_ptr(m_projectionMat));

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
	m_postProcessCubeShader->use();
	m_postProcessCubeShader->setMatrix("modelMat", glm::value_ptr(modelMat));

	m_postProcessCubeVAO->bindVAO();
	m_postProcessCubeVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void AdvancedExerciceState::drawFramebuffer()
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
	m_quadVAO->bindTexture();
	m_sceneFramebuffer->bindTexture(); // use the color attachment texture as the texture of the quad plane
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
}

