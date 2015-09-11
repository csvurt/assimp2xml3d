#pragma once
#include "assimp/scene.h"
#include <vector>

class XML3DDataConverter {

public:

	static std::string toXml3dString(const aiMatrix4x4& m);
	static std::string toXml3dString(aiVector3D* v, unsigned int len, bool toVec2 = false);
	static std::string toXml3dString(aiColor4D* v, unsigned int len, bool toVec3 = false);
	static std::string toXml3dString(float* v, const int len);
	static std::string toXml3dString(int* v, const int len);
	static std::string toXml3dString(std::vector<int>* v);
	static std::string toXml3dString(std::vector<aiVector3D*>* v);
	static std::string toXml3dString(std::vector<aiQuaternion*>* v);
	static std::string toXml3dString(aiFace* f, unsigned int len);
	static std::string toXml3dString(std::vector<aiMatrix4x4>* v);

	static void stringToHTMLId(aiString& ai);

	static float clampZero(float v);

};