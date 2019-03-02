#pragma once

#include <vector>
#include <string>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <glm/glm.hpp>

#include "Mesh.h"
#include "Texture.h"

class Model
{
public:
	Model();

	void LoadModel(const std::string& fileName);
	void RenderModel();
	void ClearModel();

	~Model();
	std::vector<GLfloat> publicVertices;
	//std::vector<glm::vec3> publicVertices;
	std::vector<unsigned int> publicIndices;

	int vCount;
	int iCount;


private:

	void LoadNode(aiNode *node, const aiScene *scene);
	void LoadMesh(aiMesh *mesh, const aiScene *scene);
	void LoadMaterials(const aiScene *scene);

	std::vector<Mesh*> meshList;
	std::vector<Texture*> textureList;
	std::vector<unsigned int> meshToTex;
};

