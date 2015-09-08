#include "bone.h"
#include "../data_converter.h"

XML3DBone::XML3DBone() {};

XML3DBone::XML3DBone(XML3DBone* parent, aiNode* node) :
mParent(parent),
mSceneNode(node) {
	mTransformation = &node->mTransformation;
}

XML3DBone::~XML3DBone() {

}

void XML3DBone::createDebugXML(tinyxml2::XMLElement* container) {
	tinyxml2::XMLElement* boneData = container->GetDocument()->NewElement("bone");
	boneData->SetAttribute("name", mSceneNode->mName.C_Str());
	boneData->SetAttribute("transform", XML3DDataConverter::toXml3dString(*mTransformation).c_str());

	auto it = mChildren.begin();
	while (it != mChildren.end()) {
		it->createDebugXML(boneData);
		it++;
	}

	container->LinkEndChild(boneData);
}

XML3DBone* XML3DBone::newChild() {
	mChildren.emplace_back();
	return &mChildren.back();
}
