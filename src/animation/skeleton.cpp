#include "skeleton.h"
#include "../logger.h"


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
