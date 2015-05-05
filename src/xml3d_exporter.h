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
	void Export(tinyxml2::XMLElement* parent, aiNode* an, aiMatrix4x4* parentTransform);
	void Export(tinyxml2::XMLElement* parent, aiMaterial* amat);

	std::string toXml3dString(aiMatrix4x4* m);
	std::string toXml3dString(aiVector3D* v, unsigned int len, bool toVec2 = false);
	std::string toXml3dString(aiColor4D* v, unsigned int len, bool toVec3 = false);
	std::string toXml3dString(aiFace* f, unsigned int len);
	void stringToHTMLId(aiString* str);

private:
	const char* filename;
	unsigned int mChangedNamesCounter = 0;
	std::unordered_map<std::string, char> usedNames;

	void processTexturesForMaterial(tinyxml2::XMLElement* material, aiMaterial* amat);
	tinyxml2::XMLElement* processTexture(aiMaterial* amat, aiTextureType texType);
	std::string mapModeToString(int mode);
};



