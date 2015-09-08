#include "skeleton.h"
#include "../logger.h"


XML3DSkeleton::XML3DSkeleton(XML3DExporter* ex, aiNode* root) :
xml3d(ex)
{
	mRootBone = XML3DBone(NULL, root);
	createBoneStructureRecursive(&mRootBone, root);
}

XML3DSkeleton::~XML3DSkeleton() {

}

void XML3DSkeleton::createBoneStructureRecursive(XML3DBone* currentBone, aiNode* currentBoneNode) {
	currentBone->mSceneNode = currentBoneNode;
	currentBone->mTransformation = &currentBoneNode->mTransformation;

	for (unsigned int i = 0; i < currentBoneNode->mNumChildren; i++) {
		XML3DBone* childBone = currentBone->newChild();
		createBoneStructureRecursive(childBone, currentBoneNode->mChildren[i]);
	}
}

void XML3DSkeleton::createDebugOutput(tinyxml2::XMLElement* container) {
	mRootBone.createDebugXML(container);
}
