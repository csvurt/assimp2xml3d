#pragma once

#include "xml3d_exporter.h"

static const std::unordered_map<aiTextureType, std::string> SupportedTextureTypes = {
	{ aiTextureType_DIFFUSE, "diffuseTexture" },
	{ aiTextureType_SPECULAR, "specularTexture" },
	{ aiTextureType_EMISSIVE, "emissiveTexture" }
};

class XML3DMaterialExporter {

public:
	XML3DMaterialExporter(XML3DExporter* ex, aiMaterial* mat);
	~XML3DMaterialExporter();

	tinyxml2::XMLElement* getMaterial();

private:
	XML3DExporter* xml3d;
	aiMaterial* aMat;

	void addTexturesToMaterial(tinyxml2::XMLElement* matElement);
	tinyxml2::XMLElement* processTexture(aiTextureType texType);
	std::string mapModeToString(int mode);
};