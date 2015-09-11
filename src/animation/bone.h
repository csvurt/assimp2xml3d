#pragma once
#include <assimp/scene.h>
#include <vector>
#include "tinyxml2.h"

class XML3DBone {

public:
	const XML3DBone* mParent;
	const aiNode* mSceneNode;
	aiMatrix4x4 mLocalTransformation;
	aiMatrix4x4 mGlobalTransformation;
	aiMatrix4x4 mInverseBindPose;
	std::vector<XML3DBone*> mChildren;
	std::string mName;

	XML3DBone();
	XML3DBone(const XML3DBone* parent, const aiNode* node);
	~XML3DBone();

	void createDebugXML(tinyxml2::XMLElement* container);
	XML3DBone* newChild(const aiNode* node);
	XML3DBone* findBoneWithName(const std::string& name);


};
