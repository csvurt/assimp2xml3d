#pragma once

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/scene.h>
#include <unordered_map>
#include "tinyxml2.h"

class XML3DExporter {
public:
	XML3DExporter(const aiScene* scene, const char* filename);
	~XML3DExporter();

	aiScene* scene;
	tinyxml2::XMLDocument doc;

	void writeFile();
	void Export();
	void Export(tinyxml2::XMLElement* parent, aiNode* an, const aiMatrix4x4& parentTransform);

	void stringToHTMLId(aiString& str, bool checkForDuplicates = true);

private:
	const char* filename;
	unsigned int mChangedNamesCounter = 0;
	std::unordered_map<std::string, char> usedNames;
	unsigned int mNumberOfMeshesExported = 0;
	unsigned int mNumberOfMaterialsExported = 0;

	void removeDummyMaterial(aiScene* scene);

};
