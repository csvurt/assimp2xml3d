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

	std::vector<aiAnimation*> mAnimations;

	void createDebugOutput(tinyxml2::XMLElement* container);
	void createBoneData(tinyxml2::XMLElement* container);
	XML3DBone* getBoneWithName(const std::string& name);
	int getIndexForBone(const std::string& name);
	unsigned int getNumberOfBones();
	void addAnimation(aiAnimation* anim);

private:
	XML3DBone mRootBone;
	std::string mSkeletonName; // HTML sanitised copy of the root bone name
	std::vector<std::string> mBoneIndexVector;

	void createBoneStructureRecursive(XML3DBone* currentBone, const aiNode* currentBoneNode);
	void createBoneParentsElement(tinyxml2::XMLElement* dataElement, std::vector<int>* boneParents);
	void createInverseBindPoseElement(tinyxml2::XMLElement* dataElement, std::vector<aiMatrix4x4>* invBindPose);
};
