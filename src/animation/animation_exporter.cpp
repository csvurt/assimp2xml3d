#include "animation_exporter.h"
#include "../mesh_exporter.h"
#include "../logger.h"
#include "../data_converter.h"

XML3DAnimationExporter::XML3DAnimationExporter() {};

XML3DAnimationExporter::XML3DAnimationExporter(const aiScene* s) :
scene(s)
{
	mSkeletons = std::vector<XML3DSkeleton>(0);
}

void XML3DAnimationExporter::discoverBone(std::string& name) {
	mDiscoveredBoneNames.insert(name);
}

bool XML3DAnimationExporter::isKnownBone(std::string& name) {
	auto it = mDiscoveredBoneNames.find(name);
	return it != mDiscoveredBoneNames.end();
}

void XML3DAnimationExporter::processSkeleton(aiNode* rootNode) {
	mSkeletons.emplace_back(rootNode);
}

XML3DSkeleton* XML3DAnimationExporter::findSkeletonForBoneName(std::string& name) {
	for (auto it = mSkeletons.begin(); it != mSkeletons.end(); ++it) {
		XML3DBone* matchingBone = it->getBoneWithName(name);
		if (matchingBone != NULL) {
			return &(*it);
		}
	}
	return NULL;
}

void XML3DAnimationExporter::exportSkeletons(tinyxml2::XMLElement* container) {
	for (auto it = mSkeletons.begin(); it != mSkeletons.end(); ++it) {
		it->createBoneData(container);
		it->createDebugOutput(container);
	}
}

void XML3DAnimationExporter::exportMeshAnimationData(XML3DMeshExporter* meshExporter) {
	if (meshExporter->hasBones()) {
		std::string firstBoneName(meshExporter->aMesh->mBones[0]->mName.C_Str());
		XML3DSkeleton* matchingSkeleton = findSkeletonForBoneName(firstBoneName);
		if (matchingSkeleton != NULL) {
			addAnimationDataToMeshData(*meshExporter, *matchingSkeleton);
		}
		else {
			Logger::Debug("Could not find matching skeleton for bone with name " + firstBoneName);
		}
	}
}

void XML3DAnimationExporter::exportAnimationKeyframes(tinyxml2::XMLElement* container) {
	for (int i = 0; i < scene->mNumAnimations; i++) {
		aiAnimation* anim = scene->mAnimations[i];
		tinyxml2::XMLElement* animData = container->GetDocument()->NewElement("assetdata");
		//xml3d->stringToHTMLId(mAnim->mName);
		animData->SetAttribute("name", anim->mName.C_Str());

		processAnimationKeyframes(anim, animData);
		createAnimationKeyBlock(anim, container);
		container->LinkEndChild(animData);
	}
}

void XML3DAnimationExporter::addAnimationDataToMeshData(XML3DMeshExporter& meshExporter, XML3DSkeleton& skeleton) {
		std::vector<float> boneWeights(meshExporter.aMesh->mNumVertices * 4); //numVertices * float4
		std::vector<int> boneIndices(meshExporter.aMesh->mNumVertices * 4);	//numVertices * int4
		
		for (unsigned int i = 0; i < meshExporter.aMesh->mNumBones; i++) {
			aiBone* bone = meshExporter.aMesh->mBones[i];
			std::string boneName(bone->mName.C_Str());
			int boneIndex = skeleton.getIndexForBone(boneName);
			if (boneIndex < 0) {
				Logger::Debug("There was a bone in a mesh that does not appear in the skeleton hierarchy, " + boneName);
				continue;
			}

			XML3DBone* xml3dBone = skeleton.getBoneWithName(boneName);
			xml3dBone->mInverseBindPose = bone->mOffsetMatrix;
			
			// Each vertex can be affected by up to 4 bones according to the boneWeight of each, we loop through all bones and collect these weights
			for (unsigned int w = 0; w < bone->mNumWeights; w++) {
				aiVertexWeight weight = bone->mWeights[w];
				int boneWeightIndex = weight.mVertexId * 4;
				int iteration = 0;
				while (boneWeights[boneWeightIndex] > 0 && iteration <= 3) { // Each vertex has 4 "slots" for bone weight contributions, find a free one
					boneWeightIndex++;
					iteration++;
				}
				if (iteration >= 3) {
					continue; //Reached maximum number of weight contributions for this vertex
			}
			boneWeights[boneWeightIndex] = weight.mWeight;
			boneIndices[boneWeightIndex] = boneIndex;
		}
	}
		
	createBoneWeightElement(meshExporter.mDataElement, boneWeights);
	createBoneIndexElement(meshExporter.mDataElement, boneIndices);
	//meshExporter.mDataElement->SetAttribute("includes", nameOfBoneDataElement.c_str());
	meshExporter.mDataElement->SetAttribute("compute", "dataflow['protos.xml#animateMesh']");

}

void XML3DAnimationExporter::createBoneWeightElement(tinyxml2::XMLElement* dataElement, std::vector<float>& boneWeights) {
	tinyxml2::XMLElement* boneWeightElement = dataElement->GetDocument()->NewElement("float4");
	boneWeightElement->SetAttribute("name", "boneWeight");
	boneWeightElement->SetText(XML3DDataConverter::toXml3dString(&boneWeights).c_str());
	dataElement->LinkEndChild(boneWeightElement);
}

void XML3DAnimationExporter::createBoneIndexElement(tinyxml2::XMLElement* dataElement, std::vector<int>& boneIndicies) {
	tinyxml2::XMLElement* boneIndexElement = dataElement->GetDocument()->NewElement("int4");
	boneIndexElement->SetAttribute("name", "boneIndex");
	boneIndexElement->SetText(XML3DDataConverter::toXml3dString(&boneIndicies).c_str());
	dataElement->LinkEndChild(boneIndexElement);
}

void XML3DAnimationExporter::createAnimationKeyBlock(aiAnimation* anim, tinyxml2::XMLElement* container) {
	std::string animName = std::string(anim->mName.C_Str()) + "_key";
	tinyxml2::XMLElement* animKeyBlock = container->GetDocument()->NewElement("assetdata");
	animKeyBlock->SetAttribute("name", animName.c_str());

	tinyxml2::XMLElement* animKey = container->GetDocument()->NewElement("float");
	animKey->SetAttribute("name", "key");
	animKey->SetText(0);
	animKeyBlock->LinkEndChild(animKey);

	container->InsertFirstChild(animKeyBlock);
}

void XML3DAnimationExporter::processAnimationKeyframes(aiAnimation* anim, tinyxml2::XMLElement* animData) {
	int numKeyframes = getNumberOfKeyframesInAnimation(anim);
	float keyframeDuration = (anim->mDuration / numKeyframes) / anim->mTicksPerSecond;
	float currentKey = 0.f;
	aiVector3D zerov(0.f, 0.f, 0.f);
	aiQuaternion zeroq(1.f, 0.f, 0.f, 0.f);

	std::string boneName = std::string(anim->mChannels[0]->mNodeName.C_Str());
	XML3DSkeleton* skeleton = findSkeletonForBoneName(boneName);
	if (skeleton == NULL) {
		Logger::Debug("Could not find matching skeleton for the animation " + std::string(anim->mName.C_Str()));
		return;
	}

	unsigned int numBones = skeleton->getNumberOfBones();
	for (unsigned int i = 0; i < numKeyframes; i++) {
		std::vector<aiVector3D*> translations(numBones, &zerov);
		std::vector<aiQuaternion*> rotations(numBones, &zeroq);

		for (unsigned int ch = 0; ch < anim->mNumChannels; ch++) {
			//Each channel holds a set of keyframes for a single bone, more or less
			aiNodeAnim* channel = anim->mChannels[ch];
			std::string boneName = std::string(channel->mNodeName.C_Str());
			std::string strippedBoneName = stripAssimpSuffixFromBoneName(boneName);
			int boneIndex = skeleton->getIndexForBone(strippedBoneName);

			if (channel->mNumPositionKeys >= numKeyframes) {
				translations[boneIndex] = &channel->mPositionKeys[i].mValue;
			}
			if (channel->mNumRotationKeys >= numKeyframes) {
				rotations[boneIndex] = &channel->mRotationKeys[i].mValue;
			}
		}

		addKeyframeElementsToData(animData, translations, rotations, currentKey);
		currentKey += keyframeDuration;
	}
}

std::string XML3DAnimationExporter::stripAssimpSuffixFromBoneName(std::string& boneName) {
	// Depending on the import format Assimp may add a suffix to some bone names eg. <bonename>_$AssimpFbx$_<transformName>
	return boneName.substr(0, boneName.find_first_of('$') - 1); // Take the <bonename> part
}

unsigned int XML3DAnimationExporter::getNumberOfKeyframesInAnimation(aiAnimation* anim) {
	unsigned int numKeyframes = 0;
	for (unsigned int ch = 0; ch < anim->mNumChannels; ch++) {
		aiNodeAnim* channel = anim->mChannels[ch];
		unsigned int keyframes = std::max(channel->mNumPositionKeys, channel->mNumRotationKeys);
		numKeyframes = std::max(numKeyframes, keyframes);
	}
	return numKeyframes;
}

void XML3DAnimationExporter::addKeyframeElementsToData(tinyxml2::XMLElement* animData, std::vector<aiVector3D*>& translations, std::vector<aiQuaternion*>& rotations, float key) {
	tinyxml2::XMLElement* translationKey = animData->GetDocument()->NewElement("float3");
	translationKey->SetAttribute("name", "translation");
	translationKey->SetAttribute("key", key);
	translationKey->SetText(XML3DDataConverter::toXml3dString(&translations).c_str());

	tinyxml2::XMLElement* rotationKey = animData->GetDocument()->NewElement("float4");
	rotationKey->SetAttribute("name", "rotation");
	translationKey->SetAttribute("key", key);
	rotationKey->SetText(XML3DDataConverter::toXml3dString(&rotations).c_str());

	animData->LinkEndChild(translationKey);
	animData->LinkEndChild(rotationKey);
}
