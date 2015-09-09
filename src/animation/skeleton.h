#pragma once

#include <assimp/scene.h>
#include <unordered_map>
#include "tinyxml2.h"
#include "bone.h"

class XML3DSkeleton {
public:

	XML3DSkeleton();
	XML3DSkeleton(aiNode* root);
	~XML3DSkeleton();

	void createDebugOutput(tinyxml2::XMLElement* container);
	void createBoneData(tinyxml2::XMLElement* container);
	XML3DBone* getBoneWithName(std::string& name);
	int getIndexForBone(std::string& name);

private:
	XML3DBone mRootBone;
	std::string mSkeletonName; // HTML sanitised copy of the root bone name
	std::vector<std::string> mBoneIndexVector;

	void createBoneStructureRecursive(XML3DBone* currentBone, aiNode* currentBoneNode);
	void createBoneParentsElement(tinyxml2::XMLElement* dataElement, std::vector<int>* boneParents);
	void createBoneTransformsElement(tinyxml2::XMLElement* dataElement, std::vector<aiMatrix4x4>* boneTransforms);
	void createInverseBindPoseElement(tinyxml2::XMLElement* dataElement, std::vector<aiMatrix4x4>* invBindPose);
};
