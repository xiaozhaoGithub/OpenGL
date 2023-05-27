#include "Model.h"

#include <iostream>
#include <fstream>

#include "glad/glad.h"
#include "stb_image.h"
//#include "LogWrapper.h"
#include <time.h>

Model::Model(const std::string& path)
{
	clock_t start_time = clock();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		std::cout << "Model loaded failed! error info: " << importer.GetErrorString() << std::endl;
		return;
	}

	clock_t mid_time = clock();
	std::cout << "The mid run time is: " << (double)(mid_time - start_time) / CLOCKS_PER_SEC << "s" << std::endl;

	m_path = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

	clock_t end_time = clock();
	std::cout << "The total run time is: " << (double)(end_time - start_time) / CLOCKS_PER_SEC << "s" << std::endl;
}

void Model::draw(std::shared_ptr<AbstractShader> shader)
{
	for (unsigned int i = 0; i < m_meshes.size(); i++) {
		m_meshes[i].draw(shader);
	}
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		// 结点mMeshes存网格索引，场景mMeshes存的是网格对象
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh meshed = processMesh(mesh, scene);

		m_meshes.push_back(meshed);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		aiNode* cNode = node->mChildren[i];
		processNode(cNode, scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene * scene)
{
	std::vector<Vertex> vertexs;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		if (mesh->HasNormals()) {
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		// Assimp允许有8个不同的纹理坐标
		if (mesh->HasTextureCoords(0)) {
			vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex.texCoords.x = 0.0f;
			vertex.texCoords.y = 0.0f;
		}

		// tangent

		// bitangent

		vertexs.push_back(vertex);
	}

	// 现在遍历每个网格的面(面是一个网格的图元，如三角形)并检索相应的顶点索引。
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	} 

	// 对网格提供一些细节，我们还需要处理网格的材质(即获取颜色/纹理)
	if (mesh->mMaterialIndex >= 0) {
		// 像结点一样、网格mMaterials材质索引，场景mMaterials存的是材质对象
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// 网格的漫反射贴图
		auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// 镜面光贴图
		auto specularMaps= loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	
		// 反射光贴图
		auto reflectionMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflection");
		textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());
	}

	return Mesh(vertexs, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, const std::string& name)
{
	std::vector<Texture> textures;

	// 一个材质可能存在多种纹理单元
	unsigned int count = material->GetTextureCount(type);
	for (unsigned int i = 0; i < count; i++) {
		aiString path;
		material->GetTexture(type, i, &path);

		// 此处假设每一个文纹理文件名都不一致的情况
		auto iter = m_textureLoadedMap.find(path.C_Str());
		if (iter != m_textureLoadedMap.end()) {
			textures.push_back(m_textureLoadedMap[path.C_Str()]);
		}
		else {
			Texture texture;
			texture.id = textureFromFile(m_path, path.C_Str());
			texture.type = name;
			textures.push_back(texture);

			m_textureLoadedMap[path.C_Str()] = texture;
		}
	}
	
	return textures;
}

unsigned int Model::textureFromFile(const std::string& path, char const* name)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	std::string filename = path + "/" + name;

	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); // 在生成纹理之后调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Texture failed to load at path: " << filename << std::endl;
	}

	stbi_image_free(data);

	return textureID;
}
