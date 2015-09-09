#pragma once

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/scene.h>
#include <unordered_map>
#include <set>
#include "tinyxml2.h"
#include "animation/animation_exporter.h"

class XML3DMeshExporter;

class XML3DExporter {
public:
	XML3DExporter(const aiScene* scene, const char* filename);
	~XML3DExporter();

	aiScene* scene;
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* mAsset;
	tinyxml2::XMLElement* mDefs;
	XML3DAnimationExporter mAnimationExporter;

	void writeFile();
	void Export();

	void stringToHTMLId(aiString& str);

private:
	const char* filename;
	unsigned int mChangedNamesCounter = 0;
	std::unordered_map<std::string, char> usedNames;
	std::vector<XML3DMeshExporter> mMeshExporters;	

	void removeDummyMaterial(aiScene* scene);
	void createAssetElements();
	void processSceneTree(tinyxml2::XMLElement* parent, aiNode* an, const aiMatrix4x4& parentTransform);

	void exportMeshAnimationData();
	void exportMeshDataAndDiscoverBones();
	void exportMaterials();
	void exportSkeletons();

};
