#pragma once

#include <assimp/scene.h>
#include <map>
#include "tinyxml2.h"
#include "bone.h"
#include "../xml3d_exporter.h"

class XML3DSkeleton {
public:

	XML3DSkeleton();
	XML3DSkeleton(XML3DExporter* ex, aiNode* root);
	~XML3DSkeleton();

	void createDebugOutput(tinyxml2::XMLElement* container);
	tinyxml2::XMLElement* getBoneData();

private:
	XML3DExporter* xml3d;
	XML3DBone mRootBone;
	std::string mSkeletonName; // HTML sanitised copy of the root bone name
	std::vector<std::string> mBoneIndexVector;
	std::unordered_map<std::string, XML3DBone*> mBoneNameMap;

	void createBoneStructureRecursive(XML3DBone* currentBone, aiNode* currentBoneNode);
	void createBoneParentsElement(tinyxml2::XMLElement* dataElement, std::vector<int>* boneParents);
	void createBoneTransformsElement(tinyxml2::XMLElement* dataElement, std::vector<aiMatrix4x4>* boneTransforms);
};
