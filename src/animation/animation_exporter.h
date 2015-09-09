#pragma once
#include <vector>
#include <set>
#include "assimp\scene.h"
#include "skeleton.h"

class XML3DMeshExporter;

class XML3DAnimationExporter {
public:
	XML3DAnimationExporter();
	XML3DAnimationExporter(const aiScene* s);
	
	void exportSkeletons(tinyxml2::XMLElement* container);
	void exportMeshAnimationData(XML3DMeshExporter* meshExporter);
	void exportAnimationKeyframes(tinyxml2::XMLElement* container);

	void discoverBone(std::string& name);
	bool isKnownBone(std::string& name);
	void processSkeleton(aiNode* rootNode);
	XML3DSkeleton* findSkeletonForBoneName(std::string& name);

private:
	aiScene* scene;
	std::vector<XML3DSkeleton> mSkeletons;
	std::set<std::string> mDiscoveredBoneNames;

	void addAnimationDataToMeshData(XML3DMeshExporter& meshExporter, XML3DSkeleton& skeleton);
	void createBoneWeightElement(tinyxml2::XMLElement* dataElement, std::vector<float>& boneWeights);
	void createBoneIndexElement(tinyxml2::XMLElement* dataElement, std::vector<int>& boneIndex);

	
};