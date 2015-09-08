#pragma once
#include "assimp/scene.h"
#include <vector>

class XML3DDataConverter {

public:

	static std::string toXml3dString(const aiMatrix4x4& m);
	static std::string toXml3dString(aiVector3D* v, unsigned int len, bool toVec2 = false);
	static std::string toXml3dString(aiColor4D* v, unsigned int len, bool toVec3 = false);
	static std::string toXml3dString(std::vector<float>* v, unsigned int len);
	static std::string toXml3dString(std::vector<unsigned int>* v, unsigned int len);
	static std::string toXml3dString(aiFace* f, unsigned int len);
	static std::string toXml3dString(std::vector<aiMatrix4x4>* v, unsigned int numEntries);

};