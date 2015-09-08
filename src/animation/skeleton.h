#pragma once

#include <assimp/scene.h>
#include <map>
#include "tinyxml2.h"
#include "bone.h"
#include "../xml3d_exporter.h"

class XML3DSkeleton {
public:

	XML3DSkeleton(XML3DExporter* ex, aiNode* root);
	~XML3DSkeleton();

	void createDebugOutput(tinyxml2::XMLElement* container);

private:
	XML3DExporter* xml3d;
	XML3DBone mRootBone;
	std::string mSkeletonName; // HTML sanitised copy of the root bone name

	void createBoneStructureRecursive(XML3DBone* currentBone, aiNode* currentBoneNode);
};
