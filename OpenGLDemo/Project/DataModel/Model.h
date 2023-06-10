#ifndef MODEL_H_
#define MODEL_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.h"
#include "Mesh.h"

/**
 * @brief ͨ��ģ�ͼ���
 */
class Model
{
public:
	Model(const std::string& path);

	void draw(std::shared_ptr<AbstractShader> shader);
	void draw(std::shared_ptr<AbstractShader> shader, unsigned int instanceNum);

	void setRuleCb(std::function<void()> rule);

private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, const std::string& name);

	unsigned int textureFromFile(const std::string& path, char const* name);

private:
	std::string m_path;
	std::vector<Mesh> m_meshes;

	// Use flyweight pattern to optimize
	// �ٷ��Ż�ʹ��vector�������Ҹ���ʱ���˴�ʹ�ù�ϣ��
	std::unordered_map<std::string, Texture> m_textureLoadedMap;
};

#endif

