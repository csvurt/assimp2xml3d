#include "material_exporter.h"
#include "logger.h"
#include "data_converter.h"
#include <assimp/../../code/BoostWorkaround/boost/lexical_cast.hpp>

static const std::map<aiTextureType, std::string> SupportedTextureTypes = {
	{ aiTextureType_DIFFUSE, "diffuseTexture" },
	{ aiTextureType_SPECULAR, "specularTexture" },
	{ aiTextureType_EMISSIVE, "emissiveTexture" }
};

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
	xml3d->stringToHTMLId(name);
	material->SetAttribute("id", name.C_Str());
	material->SetAttribute("model", "urn:xml3d:material:phong"); //TODO: Choose right shading model

	// For now we only handle properties that the default XML3D material shaders can work with
	processDiffuseColor(material);
	processSpecularColor(material);
	processEmissiveColor(material);
	processShininess(material);
	processOpacity(material, name);
	addTexturesToMaterial(material);

	return material;
}

void XML3DMaterialExporter::processDiffuseColor(tinyxml2::XMLElement* matElement) {
	aiColor4D dColor;
	if (AI_SUCCESS != aMat->Get(AI_MATKEY_COLOR_DIFFUSE, dColor)) {
		return;
	}
	tinyxml2::XMLElement* diffuseColor = xml3d->doc.NewElement("float3");
	diffuseColor->SetAttribute("name", "diffuseColor");
	diffuseColor->SetText(XML3DDataConverter::toXml3dString(&dColor, 1, true).c_str());
	matElement->LinkEndChild(diffuseColor);
}

void XML3DMaterialExporter::processSpecularColor(tinyxml2::XMLElement* matElement) {
	aiColor4D sColor;
	if (AI_SUCCESS != aMat->Get(AI_MATKEY_COLOR_SPECULAR, sColor)) {
		return; //Let XML3D supply the default value
	}
	tinyxml2::XMLElement* specularColor = xml3d->doc.NewElement("float3");
	specularColor->SetAttribute("name", "specularColor");
	specularColor->SetText(XML3DDataConverter::toXml3dString(&sColor, 1, true).c_str());
	matElement->LinkEndChild(specularColor);
}

void XML3DMaterialExporter::processEmissiveColor(tinyxml2::XMLElement* matElement) {
	aiColor4D eColor;
	if (AI_SUCCESS != aMat->Get(AI_MATKEY_COLOR_EMISSIVE, eColor)) {
		return;
	}
	tinyxml2::XMLElement* emissiveColor = xml3d->doc.NewElement("float3");
	emissiveColor->SetAttribute("name", "emissiveColor");
	emissiveColor->SetText(XML3DDataConverter::toXml3dString(&eColor, 1, true).c_str());
	matElement->LinkEndChild(emissiveColor);
}

void XML3DMaterialExporter::processShininess(tinyxml2::XMLElement* matElement) {
	float s;
	if (AI_SUCCESS != aMat->Get(AI_MATKEY_SHININESS, s)) {
		return;
	}
	tinyxml2::XMLElement* shininess = xml3d->doc.NewElement("float");
	shininess->SetAttribute("name", "shininess");
	shininess->SetText(boost::lexical_cast<std::string>(s).c_str());
	matElement->LinkEndChild(shininess);
}

void XML3DMaterialExporter::processOpacity(tinyxml2::XMLElement* matElement, aiString& materialName) {
	float o;
	if (AI_SUCCESS != aMat->Get(AI_MATKEY_OPACITY, o)) {
		return;
	}
	if (o <= 0.0001f) {
		Logger::Warn("Material with name '" + std::string(materialName.C_Str()) + "' has an opacity value of 0. Meshes rendered with this material will be fully transparent!");
	}
	tinyxml2::XMLElement* opacity = xml3d->doc.NewElement("float");
	opacity->SetAttribute("name", "opacity");
	opacity->SetText(boost::lexical_cast<std::string>(o).c_str());
	matElement->LinkEndChild(opacity);
}

void XML3DMaterialExporter::addTexturesToMaterial(tinyxml2::XMLElement* matElement) {
	for (auto it = SupportedTextureTypes.begin(); it != SupportedTextureTypes.end(); ++it) {
		if (aMat->GetTextureCount(it->first) > 0) {
			tinyxml2::XMLElement* texElement = processTexture(it->first);
			if (texElement) {
				texElement->SetAttribute("name", it->second.c_str());
				matElement->LinkEndChild(texElement);
			}
			if (aMat->GetTextureCount(it->first) > 1) {
				// XML3D only supports 1 texture per channel by default, so for now we limit the exporter to that
				Logger::Warn("Found multiple textures for channel " + std::string(it->second.c_str()) + ", only the first texture will be retained.");
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
		Logger::Warn("Could not process a texture for material " + std::string(matName.C_Str()));
		return NULL;
	}

	tinyxml2::XMLElement* texElement = xml3d->doc.NewElement("texture");
	std::stringstream ss;
	ss << mapModeToString(wrapS) << " " << mapModeToString(wrapT);
	texElement->SetAttribute("wrap", ss.str().c_str());
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
		Logger::Warn("Unsupported texture wrapping mode encountered. Valid modes are 'clamp' or 'wrap/repeat'. Defaulting to clamp.");
		return "clamp";
	}
}
