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