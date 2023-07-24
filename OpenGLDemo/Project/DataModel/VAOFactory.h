#ifndef VAOFACTORY_H_
#define VAOFACTORY_H_

#include <string>
#include <vector>
#include <utility>

#include "Shader.h"
#include "Singleton.h"
#include "glad/glad.h"
// 请确认是在包含GLFW的头文件之前包含了GLAD的头文件。GLAD的头文件包含了正确的OpenGL头文件（例如GL/gl.h），
// 所以需要在其它依赖于OpenGL的头文件之前包含GLAD。

class AbstractVAO
{
public:
	AbstractVAO(): m_id(0) { }
	virtual ~AbstractVAO() { glDeleteVertexArrays(1, &m_id); }

	void bindVAO();

	void insertTexture(unsigned int type, unsigned int id);

	/** 
	 * @brief 每次渲染前，需绑定待渲染的纹理对象，就像绑定VAO一样
	 */
	virtual void bindTexture();

protected:
	unsigned int m_id;

private:
	std::vector<std::pair<unsigned int, unsigned int> > m_textures;
};

class TriangleVAO : public AbstractVAO
{
public:
	TriangleVAO() {}
	TriangleVAO(unsigned int id) { m_id = id; }
	~TriangleVAO() {}
};


// abstract factory pattern
class AbstractVAOFactory
{
public:
	AbstractVAOFactory() {}

	virtual std::shared_ptr<AbstractVAO> createNormalVAO() = 0;
	virtual unsigned int createSpecialVAO() { return 0; };
	virtual unsigned int createAdjacentVAO() { return 0; };
	virtual std::pair<unsigned int, unsigned int> createAdjacent2VAO() { return std::pair<unsigned int, unsigned int>(); };
	virtual unsigned int createRichVAO() { return 0; };
	virtual std::shared_ptr<AbstractVAO> creatTextureVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> creatMixTextureUnitVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createTransformVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> create3DVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createLightVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createTargetVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createLightMapTargetVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createCubeVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createNormalCubeVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createAdvancedTargetVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createTexCubeVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createPlaneVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createPlaneVAO(char const* path) { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createVPlaneVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createFloorVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createFloorVAO(char const* path) { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createVegetationVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createWindowVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createQuadVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createQuadColorVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createPostProcessVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createCubeMapVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createSkyboxVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createRelectedCubeVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createPointVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createPointsVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createInstancedVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createTangentVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createNormalMapVAO() { return nullptr; }
	virtual std::shared_ptr<AbstractVAO> createParallaxMapVAO() { return nullptr; }
};

class TriangleVAOFactory : public AbstractVAOFactory
{
	SINGLETON_DECLARE(TriangleVAOFactory)
public:
	TriangleVAOFactory() {}

	std::shared_ptr<AbstractVAO> createNormalVAO() override;
	unsigned int createSpecialVAO() override;
	unsigned int createAdjacentVAO() override;
	std::pair<unsigned int, unsigned int> createAdjacent2VAO() override;
	unsigned int createRichVAO() override;
	std::shared_ptr<AbstractVAO> creatTextureVAO() override;
	std::shared_ptr<AbstractVAO>  creatMixTextureUnitVAO() override;
	std::shared_ptr<AbstractVAO> createTransformVAO() override;
	std::shared_ptr<AbstractVAO> create3DVAO() override;
	std::shared_ptr<AbstractVAO> createLightVAO() override;
	std::shared_ptr<AbstractVAO> createTargetVAO() override;
	std::shared_ptr<AbstractVAO> createLightMapTargetVAO() override;
	std::shared_ptr<AbstractVAO> createCubeVAO() override;
	std::shared_ptr<AbstractVAO> createNormalCubeVAO() override;
	std::shared_ptr<AbstractVAO> createAdvancedTargetVAO() override;
	std::shared_ptr<AbstractVAO> createTexCubeVAO() override;
	std::shared_ptr<AbstractVAO> createPlaneVAO() override;
	std::shared_ptr<AbstractVAO> createPlaneVAO(char const* path) override;
	std::shared_ptr<AbstractVAO> createVPlaneVAO() override;
	std::shared_ptr<AbstractVAO> createFloorVAO() override;
	std::shared_ptr<AbstractVAO> createFloorVAO(char const* path) override;
	std::shared_ptr<AbstractVAO> createVegetationVAO() override;
	std::shared_ptr<AbstractVAO> createWindowVAO() override;
	std::shared_ptr<AbstractVAO> createQuadVAO() override;
	std::shared_ptr<AbstractVAO> createQuadColorVAO() override;
	std::shared_ptr<AbstractVAO> createPostProcessVAO() override;
	std::shared_ptr<AbstractVAO> createCubeMapVAO() override;
	std::shared_ptr<AbstractVAO> createSkyboxVAO() override;
	std::shared_ptr<AbstractVAO> createRelectedCubeVAO() override;
	std::shared_ptr<AbstractVAO> createPointVAO() override;
	std::shared_ptr<AbstractVAO> createPointsVAO() override;
	std::shared_ptr<AbstractVAO> createInstancedVAO() override;
	std::shared_ptr<AbstractVAO> createTangentVAO() override;
	std::shared_ptr<AbstractVAO> createNormalMapVAO() override;
	std::shared_ptr<AbstractVAO> createParallaxMapVAO() override;
};

class RectVAOFactory : public AbstractVAOFactory
{
public:
	RectVAOFactory() {}

	std::shared_ptr<AbstractVAO> createNormalVAO() override;
};
#endif

