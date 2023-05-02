#ifndef VAOFACTORY_H_
#define VAOFACTORY_H_

#include <string>
#include <map>

#include "Shader.h"

class AbstractVAO
{
public:
	AbstractVAO(): m_id(0) { }
	virtual ~AbstractVAO() {}

	void bindVAO();

	void insertTexture(unsigned int index, unsigned int id) { m_indexTextureMap[index] = id; }

	/** 
	 * @brief ÿ����Ⱦǰ����󶨴���Ⱦ��������󣬾����VAOһ��
	 */
	virtual void bindTexture();

protected:
	unsigned int m_id;

private:
	std::map<unsigned int, unsigned int> m_indexTextureMap;
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

	virtual unsigned int createNormalVAO() = 0;
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

protected:
	// utility function for loading a 2D texture from file
	unsigned int loadTexture(char const* path);
};

class TriangleVAOFactory : public AbstractVAOFactory
{
public:
	TriangleVAOFactory() {}

	unsigned int createNormalVAO() override;
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
};

class RectVAOFactory : public AbstractVAOFactory
{
public:
	RectVAOFactory() {}

	unsigned int createNormalVAO() override;
};
#endif

