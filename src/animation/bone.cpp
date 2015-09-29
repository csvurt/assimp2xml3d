#include "bone.h"
#include "../data_converter.h"

XML3DBone::XML3DBone() {};

XML3DBone::XML3DBone(const XML3DBone* parent, const aiNode* node) :
mParent(parent),
mSceneNode(node) {
	mChildren.reserve(node->mNumChildren);
	mName = std::string(node->mName.C_Str());
}

XML3DBone::~XML3DBone() {
	auto it = mChildren.begin();
	while (it != mChildren.end()) {
		delete *it;
		it = mChildren.erase(it);
	}
}

void XML3DBone::createDebugXML(tinyxml2::XMLElement* container) {
	tinyxml2::XMLElement* boneData = container->GetDocument()->NewElement("bone");
	boneData->SetAttribute("name", mSceneNode->mName.C_Str());
	boneData->SetAttribute("inverseBindPose", XML3DDataConverter::toXml3dString(mInverseBindPose).c_str());

	auto it = mChildren.begin();
	while (it != mChildren.end()) {
		(*it)->createDebugXML(boneData);
		it++;
	}

	container->LinkEndChild(boneData);
}

XML3DBone* XML3DBone::newChild(const aiNode* node) {
	XML3DBone* child = new XML3DBone(this, node);
	mChildren.push_back(child);
	return child;
}

XML3DBone* XML3DBone::findBoneWithName(const std::string& name) {
	if (mName == name) {
		return this;
	}
	auto it = mChildren.begin();
	while (it != mChildren.end()) {
		XML3DBone* child = (*it)->findBoneWithName(name);
		if (child != NULL) {
			return child;
		}
		it++;
	}
	return NULL;
}