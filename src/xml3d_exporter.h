#pragma once

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/scene.h>
#include <unordered_map>
#include <set>
#include "tinyxml2.h"

class XML3DMeshExporter;
class XML3DSkeleton;

class XML3DExporter {
public:
	XML3DExporter(const aiScene* scene, const char* filename);
	~XML3DExporter();

	aiScene* scene;
	tinyxml2::XMLDocument doc;

	void writeFile();
	void Export();
	void processSceneTree(tinyxml2::XMLElement* parent, aiNode* an, const aiMatrix4x4& parentTransform);
	void processAnimationData(tinyxml2::XMLElement* asset);

	void stringToHTMLId(aiString& str);
	void discoverBone(std::string& name);
	bool isKnownBone(std::string& name);

private:
	const char* filename;
	unsigned int mChangedNamesCounter = 0;
	std::unordered_map<std::string, char> usedNames;
	std::vector<XML3DMeshExporter> mMeshExporters;
	std::vector<XML3DSkeleton> mSkeletons;
	std::set<std::string> mDiscoveredBoneNames;

	void removeDummyMaterial(aiScene* scene);

};
