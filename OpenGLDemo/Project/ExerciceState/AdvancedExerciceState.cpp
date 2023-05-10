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

	m_depthTestShader = Singleton<ShaderFactory>::instance()->shaderProgram("depth_test_shader", "ShaderProgram/Advanced/texture_shader.vs", "ShaderProgram/Advanced/depth_test_shader.fs");
	m_shader = Singleton<ShaderFactory>::instance()->shaderProgram("cube_texture_shader", "ShaderProgram/Advanced/texture_shader.vs", "ShaderProgram/Advanced/texture_shader.fs");
	m_shader->use();
	m_shader->setInt("sampler1", 0);
	m_singleColorShader = Singleton<ShaderFactory>::instance()->shaderProgram("single_color_shader", "ShaderProgram/Advanced/texture_shader.vs", "ShaderProgram/Advanced/single_color_shader.fs");
	m_transparentShader = Singleton<ShaderFactory>::instance()->shaderProgram("transparent_texture_shader", "ShaderProgram/Advanced/texture_shader.vs", "ShaderProgram/Advanced/transparent_texture_shader.fs");
	m_transparentShader->use();
	m_transparentShader->setInt("sampler1", 0);

	m_vegetationPos.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	m_vegetationPos.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	m_vegetationPos.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	m_vegetationPos.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	m_vegetationPos.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

	// 必须开启缓冲测试，才能使用相关功能
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void AdvancedExerciceState::draw()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	auto cameraWrapper = Singleton<CameraWrapper>::instance();
	cameraWrapper->setCursorVisible(false);

	// 模型矩阵主要处理平移、缩放、旋转。一个顶点向量乘模型矩阵，得到世界空间中的顶点位置
	m_modelMat = glm::mat4(1.0f);

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
}

void AdvancedExerciceState::drawDepthTest()
{
	m_modelMat = glm::mat4(1.0f);
	m_modelMat = glm::translate(m_modelMat, glm::vec3(-4.0f, 0.0f, -1.0f));

	m_depthTestShader->use();
	m_depthTestShader->setMatrix("modelMat", glm::value_ptr(m_modelMat));
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
	m_modelMat = glm::mat4(1.0f);
	m_shader->setMatrix("modelMat", glm::value_ptr(m_modelMat));

	m_planeVAO->bindVAO();
	m_planeVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void AdvancedExerciceState::drawCube()
{
	m_singleColorShader->use();
	m_singleColorShader->setMatrix("viewMat", glm::value_ptr(m_viewMat));
	m_singleColorShader->setMatrix("projectionMat", glm::value_ptr(m_projectionMat));

	// cube 1
	glStencilFunc(GL_ALWAYS, 1, 0xff); // 所有的片段都应该更新模板缓冲
	glStencilMask(0xff); // 启用模板缓冲写入

	m_modelMat = glm::mat4(1.0f);
	m_modelMat = glm::translate(m_modelMat, glm::vec3(-1.0f, 0.0f, -1.0f));
	m_shader->use();
	m_shader->setMatrix("modelMat", glm::value_ptr(m_modelMat));

	m_cubeVAO->bindVAO();
	m_cubeVAO->bindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// border 1
	glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	glStencilMask(0x00);

	m_modelMat = glm::scale(m_modelMat, glm::vec3(1.1f));
	m_singleColorShader->use();
	m_singleColorShader->setMatrix("modelMat", glm::value_ptr(m_modelMat));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// cube 2
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilMask(0xff);

	m_modelMat = glm::mat4(1.0f);
	m_modelMat = glm::translate(m_modelMat, glm::vec3(2.0f, 0.0f, 0.0f));
	m_shader->use();
	m_shader->setMatrix("modelMat", glm::value_ptr(m_modelMat));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// border 2
	glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	glStencilMask(0x00);

	m_modelMat = glm::scale(m_modelMat, glm::vec3(1.1f));
	m_singleColorShader->use();
	m_singleColorShader->setMatrix("modelMat", glm::value_ptr(m_modelMat));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// reset
	glEnable(GL_DEPTH_TEST);
	glStencilMask(0xff);
	glStencilFunc(GL_ALWAYS, 0, 0xff);
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

