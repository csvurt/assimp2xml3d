#include "data_converter.h"
#include "logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <assimp/../../code/BoostWorkaround/boost/lexical_cast.hpp>
#include <unordered_map>

static const float EPSILON = 0.00001f;

std::string XML3DDataConverter::toXml3dString(const aiMatrix4x4& m) {
	std::stringstream ss;
	ss << "transform: matrix3d(";
	ss << std::setprecision(4)
		<< clampZero(m.a1) << ", " << clampZero(m.b1) << ", " << clampZero(m.c1) << ", " << clampZero(m.d1) << ", "
		<< clampZero(m.a2) << ", " << clampZero(m.b2) << ", " << clampZero(m.c2) << ", " << clampZero(m.d2) << ", "
		<< clampZero(m.a3) << ", " << clampZero(m.b3) << ", " << clampZero(m.c3) << ", " << clampZero(m.d3) << ", "
		<< clampZero(m.a4) << ", " << clampZero(m.b4) << ", " << clampZero(m.c4) << ", " << clampZero(m.d4) << ");";
	std::string str = ss.str();
	if (str.find("#Q") != std::string::npos) {
		Logger::Warn("Invalid transformation matrix '" + str + "', replacing with Identity.");
		return toXml3dString(aiMatrix4x4());
	}
	return ss.str();
}

std::string XML3DDataConverter::toXml3dString(aiVector3D* v, unsigned int numVertices, bool toVec2) {
	std::stringstream ss;
	if (toVec2) { // Special case for texture coordinates, vec3 in assimp -> vec2 in xml3d
		for (unsigned int i = 0; i < numVertices; i++) {
			ss << v[i].x << ' ' << v[i].y << ' ';
		}
	}
	else {
		for (unsigned int i = 0; i < numVertices; i++) {
			ss << v[i].x << ' ' << v[i].y << ' ' << v[i].z << ' ';
		}
	}

	return ss.str();
}

std::string XML3DDataConverter::toXml3dString(aiColor4D* v, unsigned int numVertices, bool toVec3) {
	std::stringstream ss;
	if (toVec3) { // Drop the alpha component as XML3D expects only RGB
		for (unsigned int i = 0; i < numVertices; i++) {
			ss << v[i].r << ' ' << v[i].g << ' ' << v[i].b << ' ';
		}
	}
	else {
		for (unsigned int i = 0; i < numVertices; i++) {
			ss << v[i].r << ' ' << v[i].g << ' ' << v[i].b << ' ' << v[i].a << ' ';
		}
	}

	return ss.str();
}



std::string XML3DDataConverter::toXml3dString(std::vector<float>* v) {
	std::stringstream ss;

	for (auto it = v->begin(); it != v->end(); it++) {
		ss << *it << ' ';
	}

	return ss.str();
}

std::string XML3DDataConverter::toXml3dString(std::vector<int>* v) {
	std::stringstream ss;

	for (auto it = v->begin(); it != v->end(); it++) {
		ss << *it << ' ';
	}

	return ss.str();
}

std::string XML3DDataConverter::toXml3dString(std::vector<aiVector3D*>* v) {
	std::stringstream ss;
	ss << std::setprecision(5);

	for (auto it = v->begin(); it != v->end(); it++) {
		ss << clampZero((*it)->x) << ' ' << clampZero((*it)->y) << ' ' << clampZero((*it)->z) << ' ';
	}

	return ss.str();
}

std::string XML3DDataConverter::toXml3dString(std::vector<aiQuaternion*>* v) {
	std::stringstream ss;
	ss << std::setprecision(5);

	for (auto it = v->begin(); it != v->end(); it++) {
		ss << clampZero((*it)->x) << ' ' << clampZero((*it)->y) << ' ' << clampZero((*it)->z) << ' ' << clampZero((*it)->w) << ' ';
	}

	return ss.str();
}

float XML3DDataConverter::clampZero(float v) {
	return abs(v) < EPSILON ? 0.f : v;
}

std::string XML3DDataConverter::toXml3dString(std::vector<aiMatrix4x4>* v) {
	std::stringstream ss;

	for (auto it = v->begin(); it != v->end(); it++) {
		aiMatrix4x4 m = *it;
		ss << std::setprecision(5)
			<< clampZero(m.a1) << " " << clampZero(m.b1) << " " << clampZero(m.c1) << " " << clampZero(m.d1) << " "
			<< clampZero(m.a2) << " " << clampZero(m.b2) << " " << clampZero(m.c2) << " " << clampZero(m.d2) << " "
			<< clampZero(m.a3) << " " << clampZero(m.b3) << " " << clampZero(m.c3) << " " << clampZero(m.d3) << " "
			<< clampZero(m.a4) << " " << clampZero(m.b4) << " " << clampZero(m.c4) << " " << clampZero(m.d4) << " ";
	}

	return ss.str();
}

std::string XML3DDataConverter::toXml3dString(aiFace* f, unsigned int numFaces) {
	std::stringstream ss;
	for (unsigned int i = 0; i < numFaces; i++) {
		ss << f[i].mIndices[0] << ' ' << f[i].mIndices[1] << ' ' << f[i].mIndices[2] << ' ';
	}

	return ss.str();
}

static int ChangedNamesCounter = 0;
static std::unordered_map<std::string, char> UsedNames;

void XML3DDataConverter::stringToHTMLId(aiString& ai) {
	// Ensure the name is not empty and is safe to use as an HTML5 id string
	std::string str(ai.C_Str());

	if (!(str.length() > 0)) {
		str = "_Generated_Name_" + boost::lexical_cast<std::string>(ChangedNamesCounter++);
	}

	std::replace(str.begin(), str.end(), ' ', '_');
	std::replace(str.begin(), str.end(), '.', '_'); // . can interfere with asset includes

	if (UsedNames.count(str) > 0) {
		str += "_" + boost::lexical_cast<std::string>(ChangedNamesCounter++);
		Logger::Warn("Renamed '" + str.substr(0, str.find_last_of("_")) + "' to '" + str + "' to avoid duplicate IDs");
	}
	UsedNames.emplace(str, 'x');

	ai.Set(str);
}