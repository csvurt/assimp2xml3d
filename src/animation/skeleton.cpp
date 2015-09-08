#include "skeleton.h"
#include "../logger.h"
#include "../data_converter.h"

const static std::string ASSIMP_FBX_SUFFIX = "$AssimpFbx$";

XML3DSkeleton::XML3DSkeleton() {};

XML3DSkeleton::XML3DSkeleton(XML3DExporter* ex, aiNode* root) :
xml3d(ex)
{
	mRootBone = XML3DBone(NULL, root);
	createBoneStructureRecursive(&mRootBone, root);
	aiString skeletonName(root->mName);
	ex->stringToHTMLId(skeletonName);
	mSkeletonName = std::string(skeletonName.C_Str());
}

XML3DSkeleton::~XML3DSkeleton() {

}

void XML3DSkeleton::createBoneStructureRecursive(XML3DBone* currentBone, aiNode* currentBoneNode) {
	currentBone->mSceneNode = currentBoneNode;
	currentBone->mLocalTransformation = currentBoneNode->mTransformation; 

	std::string nodeName = std::string(currentBoneNode->mName.C_Str());
	if (nodeName.find(ASSIMP_FBX_SUFFIX) == std::string::npos) {
		// This vector will be used to ensure the export order of the various animation datas are always consistent
		mBoneIndexVector.emplace_back(nodeName);
		mBoneNameMap.emplace(nodeName, currentBone);
	}

	if (currentBone->mParent != NULL) {
		currentBone->mGlobalTransformation = currentBone->mParent->mGlobalTransformation * currentBoneNode->mTransformation;
	}
	else {
		currentBone->mGlobalTransformation = currentBoneNode->mTransformation;
	}

	for (unsigned int i = 0; i < currentBoneNode->mNumChildren; i++) {
		XML3DBone* childBone = currentBone->newChild();
		createBoneStructureRecursive(childBone, currentBoneNode->mChildren[i]);
	}
}

void XML3DSkeleton::createDebugOutput(tinyxml2::XMLElement* container) {
	tinyxml2::XMLElement* skeleton = container->GetDocument()->NewElement("skeleton");
	skeleton->SetAttribute("name", mSkeletonName.c_str());
	mRootBone.createDebugXML(skeleton);
	container->LinkEndChild(skeleton);
}

tinyxml2::XMLElement* XML3DSkeleton::getBoneData() {
	tinyxml2::XMLElement* dataElement = xml3d->doc.NewElement("assetdata");
	dataElement->SetAttribute("name", mSkeletonName.c_str());

	int numBones = mBoneIndexVector.size();

	std::vector<int> boneParents;
	std::vector<aiMatrix4x4> boneTransforms;
	boneParents.reserve(numBones);
	boneTransforms.reserve(numBones);

	for (int i = 0; i < numBones; i++) {
		XML3DBone* bone = mBoneNameMap[mBoneIndexVector.at(i)];
		boneParents.push_back(i - 1);
		boneTransforms.push_back(bone->mGlobalTransformation);
	}

	createBoneParentsElement(dataElement, &boneParents);
	createBoneTransformsElement(dataElement, &boneTransforms);
	return dataElement;
}

void XML3DSkeleton::createBoneParentsElement(tinyxml2::XMLElement* dataElement, std::vector<int>* boneParents) {
	tinyxml2::XMLElement* boneParentsElement = xml3d->doc.NewElement("int");
	boneParentsElement->SetAttribute("name", "boneParent");
	boneParentsElement->SetText(XML3DDataConverter::toXml3dString(boneParents).c_str());
	dataElement->LinkEndChild(boneParentsElement);
}

void XML3DSkeleton::createBoneTransformsElement(tinyxml2::XMLElement* dataElement, std::vector<aiMatrix4x4>* boneTransforms) {
	tinyxml2::XMLElement* boneTransformsElement = xml3d->doc.NewElement("int");
	boneTransformsElement->SetAttribute("name", "boneTransform");
	boneTransformsElement->SetText(XML3DDataConverter::toXml3dString(boneTransforms).c_str());
	dataElement->LinkEndChild(boneTransformsElement);
}

