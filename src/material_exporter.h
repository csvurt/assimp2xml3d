#pragma once

#include "xml3d_exporter.h"

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

	void processDiffuseColor(tinyxml2::XMLElement* matElement);
	void processSpecularColor(tinyxml2::XMLElement* matElement);
	void processEmissiveColor(tinyxml2::XMLElement* matElement);
	void processShininess(tinyxml2::XMLElement* matElement);
	void processOpacity(tinyxml2::XMLElement* matElement, aiString& materialName);

};