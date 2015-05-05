#include "material_exporter.h"
#include "logger.h"
#include <assimp/../../code/BoostWorkaround/boost/lexical_cast.hpp>

XML3DMaterialExporter::XML3DMaterialExporter(XML3DExporter* ex, aiMaterial* mat) :
	xml3d(ex),
	aMat(mat)
{

}

XML3DMaterialExporter::~XML3DMaterialExporter() {

}

tinyxml2::XMLElement* XML3DMaterialExporter::getMaterial() {
	tinyxml2::XMLElement* material = xml3d->doc.NewElement("material");
	aiString name;
	aMat->Get(AI_MATKEY_NAME, name);
	xml3d->stringToHTMLId(&name);
	material->SetAttribute("id", name.C_Str());
	material->SetAttribute("script", "urn:xml3d:material:phong"); //TODO: Choose right shading model

	// For now we only handle properties that the default XML3D material shaders can work with
	tinyxml2::XMLElement* diffuseColor = xml3d->doc.NewElement("float3");
	diffuseColor->SetAttribute("name", "diffuseColor");
	aiColor4D dColor;
	aMat->Get(AI_MATKEY_COLOR_DIFFUSE, dColor);
	diffuseColor->SetText(xml3d->toXml3dString(&dColor, 1, true).c_str());
	material->LinkEndChild(diffuseColor);

	tinyxml2::XMLElement* specularColor = xml3d->doc.NewElement("float3");
	specularColor->SetAttribute("name", "specularColor");
	aiColor4D sColor;
	aMat->Get(AI_MATKEY_COLOR_SPECULAR, sColor);
	specularColor->SetText(xml3d->toXml3dString(&sColor, 1, true).c_str());
	material->LinkEndChild(specularColor);

	tinyxml2::XMLElement* emissiveColor = xml3d->doc.NewElement("float3");
	emissiveColor->SetAttribute("name", "emissiveColor");
	aiColor4D eColor;
	aMat->Get(AI_MATKEY_COLOR_EMISSIVE, eColor);
	emissiveColor->SetText(xml3d->toXml3dString(&eColor, 1, true).c_str());
	material->LinkEndChild(emissiveColor);

	tinyxml2::XMLElement* shininess = xml3d->doc.NewElement("float");
	shininess->SetAttribute("name", "shininess");
	float s;
	aMat->Get(AI_MATKEY_SHININESS, s);
	shininess->SetText(boost::lexical_cast<std::string>(s).c_str());
	material->LinkEndChild(shininess);

	tinyxml2::XMLElement* opacity = xml3d->doc.NewElement("float");
	opacity->SetAttribute("name", "transparency");
	float o;
	aMat->Get(AI_MATKEY_OPACITY, o);
	opacity->SetText(boost::lexical_cast<std::string>(1 - o).c_str());
	material->LinkEndChild(opacity);

	addTexturesToMaterial(material);

	return material; 
}

void XML3DMaterialExporter::addTexturesToMaterial(tinyxml2::XMLElement* matElement) {
	// XML3D only supports 1 texture per channel by default, so for now we limit the exporter to that
	for (auto it = SupportedTextureTypes.begin(); it != SupportedTextureTypes.end(); ++it) {
		if (aMat->GetTextureCount(it->first) > 0) {
			tinyxml2::XMLElement* texElement = processTexture(it->first);
			if (texElement) {
				texElement->SetAttribute("name", it->second.c_str());
				matElement->LinkEndChild(texElement);
			}
		}
	}
}

tinyxml2::XMLElement* XML3DMaterialExporter::processTexture(aiTextureType texType) {
	int wrapS;
	aMat->Get(AI_MATKEY_MAPPINGMODE_U(texType, 0), wrapS);
	int wrapT;
	aMat->Get(AI_MATKEY_MAPPINGMODE_V(texType, 0), wrapT);

	aiString texPath;
	aMat->Get(AI_MATKEY_TEXTURE(texType, 0), texPath);
	if (texPath.length <= 0) {
		Logger::Debug("Could not find texture path for texture channel " + boost::lexical_cast<std::string, int>(texType));
		aiString matName;
		aMat->Get(AI_MATKEY_NAME, matName);
		Logger::Warn("Could not process diffuse texture for material " + std::string(matName.C_Str()));
		return NULL;
	}

	tinyxml2::XMLElement* texElement = xml3d->doc.NewElement("texture");
	texElement->SetAttribute("wraps", mapModeToString(wrapS).c_str());
	texElement->SetAttribute("wrapt", mapModeToString(wrapT).c_str());
	tinyxml2::XMLElement* imgElement = xml3d->doc.NewElement("img");
	imgElement->SetAttribute("src", texPath.C_Str());
	texElement->LinkEndChild(imgElement);
	return texElement;
}

std::string XML3DMaterialExporter::mapModeToString(int mode) {
	switch (mode) {
	case aiTextureMapMode_Clamp:
		return "clamp";
	case aiTextureMapMode_Wrap:
		return "repeat";
	default:
		Logger::Warn("Unsupported texture wrapping mode encountered.Valid modes are 'clamp' or 'wrap/repeat'.Defaulting to clamp.");
		return "clamp";
	}
}
