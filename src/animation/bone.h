#pragma once
#include <assimp/scene.h>
#include <vector>
#include "tinyxml2.h"

class XML3DBone {

public:
	XML3DBone* mParent;
	aiNode* mSceneNode;
	aiMatrix4x4 mLocalTransformation;
	aiMatrix4x4 mGlobalTransformation;
	std::vector<XML3DBone> mChildren;

	XML3DBone();
	XML3DBone(XML3DBone* parent, aiNode* node);
	~XML3DBone();

	void createDebugXML(tinyxml2::XMLElement* container);
	XML3DBone* newChild();

private:


};
