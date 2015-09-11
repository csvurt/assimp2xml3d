#include "skeleton.h"
#include "../logger.h"
#include "../data_converter.h"

const static std::string ASSIMP_FBX_SUFFIX = "$AssimpFbx$";

XML3DSkeleton::XML3DSkeleton() {};

XML3DSkeleton::XML3DSkeleton(aiNode* root) {
	mRootBone = XML3DBone(NULL, root);
	createBoneStructureRecursive(&mRootBone, root);
	aiString skeletonName(root->mName);
	XML3DDataConverter::stringToHTMLId(skeletonName);
	mSkeletonName = std::string(skeletonName.C_Str());
}

XML3DSkeleton::~XML3DSkeleton() {

}

void XML3DSkeleton::createBoneStructureRecursive(XML3DBone* currentBone, const aiNode* currentBoneNode) {
	currentBone->mSceneNode = currentBoneNode;
	currentBone->mLocalTransformation = currentBoneNode->mTransformation; 

	std::string nodeName = std::string(currentBoneNode->mName.C_Str());
	if (nodeName.find(ASSIMP_FBX_SUFFIX) == std::string::npos) {
		// This vector will be used to ensure the export order of the various animation datas are always consistent
		mBoneIndexVector.emplace_back(nodeName);
	}

	if (currentBone->mParent != NULL) {
		currentBone->mGlobalTransformation = currentBone->mParent->mGlobalTransformation * currentBoneNode->mTransformation;
	}
	else {
		currentBone->mGlobalTransformation = currentBoneNode->mTransformation;
	}

	for (unsigned int i = 0; i < currentBoneNode->mNumChildren; i++) {
		XML3DBone* childBone = currentBone->newChild(currentBoneNode->mChildren[i]);
		createBoneStructureRecursive(childBone, currentBoneNode->mChildren[i]);
	}
}

void XML3DSkeleton::createDebugOutput(tinyxml2::XMLElement* container) {
	tinyxml2::XMLElement* skeleton = container->GetDocument()->NewElement("skeleton");
	skeleton->SetAttribute("name", mSkeletonName.c_str());
	mRootBone.createDebugXML(skeleton);
	container->LinkEndChild(skeleton);
}

void XML3DSkeleton::createBoneData(tinyxml2::XMLElement* container) {
	tinyxml2::XMLElement* dataElement = container->GetDocument()->NewElement("assetdata");
	dataElement->SetAttribute("name", mSkeletonName.c_str());

	const int numBones = mBoneIndexVector.size();

	std::vector<int> boneParents;
	std::vector<aiMatrix4x4> inverseBindPose;
	boneParents.reserve(numBones);
	inverseBindPose.reserve(numBones);

	for (int i = 0; i < numBones; i++) {
		const XML3DBone* bone = getBoneWithName(mBoneIndexVector.at(i));
		boneParents.push_back(i - 1);
		inverseBindPose.push_back(bone->mGlobalTransformation * bone->mInverseBindPose);
	}

	createBoneParentsElement(dataElement, &boneParents);
	createInverseBindPoseElement(dataElement, &inverseBindPose);

	container->InsertFirstChild(dataElement);
}

XML3DBone* XML3DSkeleton::getBoneWithName(std::string& name) {
	return mRootBone.findBoneWithName(name);
}

void XML3DSkeleton::createBoneParentsElement(tinyxml2::XMLElement* dataElement, std::vector<int>* boneParents) {
	tinyxml2::XMLElement* boneParentsElement = dataElement->GetDocument()->NewElement("int");
	boneParentsElement->SetAttribute("name", "boneParent");
	boneParentsElement->SetText(XML3DDataConverter::toXml3dString(boneParents).c_str());
	dataElement->LinkEndChild(boneParentsElement);
}

void XML3DSkeleton::createInverseBindPoseElement(tinyxml2::XMLElement* dataElement, std::vector<aiMatrix4x4>* invBindPose) {
	tinyxml2::XMLElement* invBindPoseElement = dataElement->GetDocument()->NewElement("float4x4");
	invBindPoseElement->SetAttribute("name", "inverseBindPose");
	invBindPoseElement->SetText(XML3DDataConverter::toXml3dString(invBindPose).c_str());
	dataElement->LinkEndChild(invBindPoseElement);
}

int XML3DSkeleton::getIndexForBone(std::string& name) {
	for (int i = 0; i < mBoneIndexVector.size(); i++) {
		if (mBoneIndexVector.at(i) == name) {
			return i;
		}
	}
	return -1;
}

unsigned int XML3DSkeleton::getNumberOfBones() {
	return mBoneIndexVector.size();
}

