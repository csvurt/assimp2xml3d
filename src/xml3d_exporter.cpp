
#include "xml3d_exporter.h"
#include "logger.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <assimp/../../code/BoostWorkaround/boost/lexical_cast.hpp>

namespace {
	void ExportScene(const char*, Assimp::IOSystem*, const aiScene*, const Assimp::ExportProperties*);
}


Assimp::Exporter::ExportFormatEntry Assimp2XML3D_desc = Assimp::Exporter::ExportFormatEntry(
	"xml",
	"XML representation of the scene, compatible for use with XML3D as an external asset.",
	"xml",
	ExportScene,
	0u);

namespace {
	void ExportScene(const char* file, Assimp::IOSystem* io, const aiScene* scene, const Assimp::ExportProperties*) {
		XML3DExporter exp(scene, file);
		exp.Export();
		exp.writeFile();
	}
}

XML3DExporter::XML3DExporter(const aiScene* ai, const char* file) {
	aiCopyScene(ai, &scene);
	filename = file;
}

XML3DExporter::~XML3DExporter() {
	doc.Clear();
	aiFreeScene(scene);
}

void XML3DExporter::Export() {
	tinyxml2::XMLElement* xml3d = doc.NewElement("xml3d");
	tinyxml2::XMLElement* defs = doc.NewElement("defs");
	tinyxml2::XMLElement* asset = doc.NewElement("asset");
	xml3d->InsertFirstChild(defs);
	xml3d->LinkEndChild(asset);
	doc.InsertFirstChild(xml3d);

	std::string id(filename);
	id = id.substr(0, id.find_first_of('.'));
	asset->SetAttribute("id", id.c_str());

	if (scene->HasMeshes()) {
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			Export(asset, scene->mMeshes[i]);
		}
	}

	if (scene->HasMaterials()) {
		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
			Export(defs, scene->mMaterials[i]);
		}
	}

	// Flatten scene hierarchy into a list of assetmeshes
	Export(asset, scene->mRootNode, &aiMatrix4x4());
}

void XML3DExporter::Export(tinyxml2::XMLElement* parent, aiNode* an, aiMatrix4x4* parentTransform) {
	// Flatten all non-mesh nodes while gathering the transformations 

	aiMatrix4x4 t(an->mTransformation);
	t = *parentTransform * t;

	for (unsigned int i = 0; i < an->mNumMeshes; i++) {
		Export(parent, scene->mMeshes[an->mMeshes[i]], &t);
	}

	for (unsigned int i = 0; i < an->mNumChildren; i++) {
		Export(parent, an->mChildren[i], &t);
	}
}

void XML3DExporter::Export(tinyxml2::XMLElement* parent, aiMesh* am, aiMatrix4x4* parentTransform) {
	tinyxml2::XMLElement* mesh = doc.NewElement("assetmesh");
	tinyxml2::XMLElement* tmat = doc.NewElement("float4x4");

	mesh->SetAttribute("includes", am->mName.C_Str());
	mesh->SetAttribute("type", "triangles");
	tmat->SetAttribute("name", "meshTransform");
	tmat->SetText(toXml3dString(parentTransform).c_str());

	aiMaterial* mat = scene->mMaterials[am->mMaterialIndex];
	aiString name;
	mat->Get(AI_MATKEY_NAME, name);
	mat->AddProperty(&name, AI_MATKEY_NAME);

	std::string namestr("#");
	namestr.append(name.C_Str());
	mesh->SetAttribute("material", namestr.c_str());

	mesh->LinkEndChild(tmat);
	parent->LinkEndChild(mesh);
}

void XML3DExporter::Export(tinyxml2::XMLElement* asset, aiMesh* am) {
	tinyxml2::XMLElement* data = doc.NewElement("assetdata");
	stringToHTMLId(&am->mName);
	data->SetAttribute("name", am->mName.C_Str());

	//Export indices
	tinyxml2::XMLElement* index = doc.NewElement("int");
	index->SetAttribute("name", "index");
	index->SetText(toXml3dString(am->mFaces, am->mNumFaces).c_str());
	data->LinkEndChild(index);

	//Export positions
	tinyxml2::XMLElement* pos = doc.NewElement("float3");
	pos->SetAttribute("name", "position");
	pos->SetText(toXml3dString(am->mVertices, am->mNumVertices).c_str());
	data->LinkEndChild(pos);

	//Export normals
	if (am->HasNormals()) {
		tinyxml2::XMLElement* norm = doc.NewElement("float3");
		norm->SetAttribute("name", "normal");
		norm->SetText(toXml3dString(am->mNormals, am->mNumVertices).c_str());
		data->LinkEndChild(norm);
	}

	//Export texcoords
	if (am->GetNumUVChannels()) {
		tinyxml2::XMLElement* tc = doc.NewElement("float2");
		tc->SetAttribute("name", "texcoord");
		// TODO: UV channel selection
		tc->SetText(toXml3dString(am->mTextureCoords[0], am->mNumVertices, true).c_str());
		data->LinkEndChild(tc);
	}

	//Export colors
	if (am->GetNumColorChannels()) {
		tinyxml2::XMLElement* color = doc.NewElement("float3");
		color->SetAttribute("name", "color");
		// TODO: Color channel selection
		color->SetText(toXml3dString(am->mColors[0], am->mNumVertices, true).c_str());
		data->LinkEndChild(color);
	}

	asset->InsertFirstChild(data);
}

void XML3DExporter::Export(tinyxml2::XMLElement* defs, aiMaterial* amat) {
	tinyxml2::XMLElement* material = doc.NewElement("material");
	aiString name;
	amat->Get(AI_MATKEY_NAME, name);
	stringToHTMLId(&name);
	material->SetAttribute("id", name.C_Str());
	material->SetAttribute("script", "urn:xml3d:material:phong"); //TODO: Choose right shading model

	// For now we only handle properties that the default XML3D material shaders can work with
	tinyxml2::XMLElement* diffuseColor = doc.NewElement("float3");
	diffuseColor->SetAttribute("name", "diffuseColor");
	aiColor4D dColor;
	amat->Get(AI_MATKEY_COLOR_DIFFUSE, dColor);
	diffuseColor->SetText(toXml3dString(&dColor, 1, true).c_str());
	material->LinkEndChild(diffuseColor);

	tinyxml2::XMLElement* specularColor = doc.NewElement("float3");
	specularColor->SetAttribute("name", "specularColor");
	aiColor4D sColor;
	amat->Get(AI_MATKEY_COLOR_SPECULAR, sColor);
	specularColor->SetText(toXml3dString(&sColor, 1, true).c_str());
	material->LinkEndChild(specularColor);

	tinyxml2::XMLElement* emissiveColor = doc.NewElement("float3");
	emissiveColor->SetAttribute("name", "emissiveColor");
	aiColor4D eColor;
	amat->Get(AI_MATKEY_COLOR_EMISSIVE, eColor);
	emissiveColor->SetText(toXml3dString(&eColor, 1, true).c_str());
	material->LinkEndChild(emissiveColor);

	tinyxml2::XMLElement* shininess = doc.NewElement("float");
	shininess->SetAttribute("name", "shininess");
	float s;
	amat->Get(AI_MATKEY_SHININESS, s);
	shininess->SetText(boost::lexical_cast<std::string>(s).c_str());
	material->LinkEndChild(shininess);

	tinyxml2::XMLElement* opacity = doc.NewElement("float");
	opacity->SetAttribute("name", "transparency");
	float o;
	amat->Get(AI_MATKEY_OPACITY, o);
	opacity->SetText(boost::lexical_cast<std::string>(1 - o).c_str());
	material->LinkEndChild(opacity);

	processTexturesForMaterial(material, amat);

	defs->LinkEndChild(material);
}

void XML3DExporter::processTexturesForMaterial(tinyxml2::XMLElement* matElement, aiMaterial* amat) {
	// XML3D only supports 1 texture per channel by default, so for now we limit the exporter to that
	if (amat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		tinyxml2::XMLElement* texElement = processTexture(amat, aiTextureType_DIFFUSE);
		if (texElement) {
			texElement->SetAttribute("name", "diffuseTexture");
			matElement->LinkEndChild(texElement);
		}
	}
	if (amat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
		tinyxml2::XMLElement* texElement = processTexture(amat, aiTextureType_SPECULAR);
		if (texElement) {
			texElement->SetAttribute("name", "specularTexture");
			matElement->LinkEndChild(texElement);
		}
	}
	if (amat->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
		tinyxml2::XMLElement* texElement = processTexture(amat, aiTextureType_EMISSIVE);
		if (texElement) {
			texElement->SetAttribute("name", "emissiveTexture");
			matElement->LinkEndChild(texElement);
		}
	}
}

tinyxml2::XMLElement* XML3DExporter::processTexture(aiMaterial* amat, aiTextureType texType) {
	int wrapS;
	amat->Get(AI_MATKEY_MAPPINGMODE_U(texType, 0), wrapS);
	int wrapT;
	amat->Get(AI_MATKEY_MAPPINGMODE_V(texType, 0), wrapT);

	aiString texPath;
	amat->Get(AI_MATKEY_TEXTURE(texType, 0), texPath);
	if (texPath.length <= 0) {
		Logger::Debug("Could not find texture path for texture channel " + boost::lexical_cast<std::string, int>(texType));
		aiString matName;
		amat->Get(AI_MATKEY_NAME, matName);
		Logger::Warn("Could not process diffuse texture for material " + std::string(matName.C_Str()));
		return NULL;
	}

	tinyxml2::XMLElement* texElement = doc.NewElement("texture");
	texElement->SetAttribute("wraps", mapModeToString(wrapS).c_str());
	texElement->SetAttribute("wrapt", mapModeToString(wrapT).c_str());
	tinyxml2::XMLElement* imgElement = doc.NewElement("img");
	imgElement->SetAttribute("src", texPath.C_Str());
	texElement->LinkEndChild(imgElement);
	return texElement;
}

std::string XML3DExporter::mapModeToString(int mode) {
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

void XML3DExporter::writeFile() {
	doc.SaveFile(filename);
}

std::string XML3DExporter::toXml3dString(aiMatrix4x4* m) {
	std::stringstream ss;
	ss  << m->a1 << ' ' << m->b1 << ' ' << m->c1 << ' ' << m->d1 << ' '
		<< m->a2 << ' ' << m->b2 << ' ' << m->c2 << ' ' << m->d2 << ' '
		<< m->a3 << ' ' << m->b3 << ' ' << m->c3 << ' ' << m->d3 << ' '
		<< m->a4 << ' ' << m->b4 << ' ' << m->c4 << ' ' << m->d4 << ' ';
	std::string str = ss.str();
	if (str.find("NAN") != std::string::npos) {
		Logger::Warn("Invalid transformation matrix, replacing with Identity: [" + str + "]");
		return toXml3dString(&aiMatrix4x4());
	}
	return ss.str();
}

std::string XML3DExporter::toXml3dString(aiVector3D* v, unsigned int numVertices, bool toVec2) {
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

std::string XML3DExporter::toXml3dString(aiColor4D* v, unsigned int numVertices, bool toVec3) {
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

std::string XML3DExporter::toXml3dString(aiFace* f, unsigned int numFaces) {
	std::stringstream ss;
	for (unsigned int i = 0; i < numFaces; i++) {
		ss << f[i].mIndices[0] << ' ' << f[i].mIndices[1] << ' ' << f[i].mIndices[2] << ' ';
	}

	return ss.str();
}

void XML3DExporter::stringToHTMLId(aiString* ai) {
	// Ensure the name is not empty and is safe to use as an HTML5 id string
	std::string str(ai->C_Str());

	if (!(str.length() > 0)) {
		str = "_Generated_Name_" + boost::lexical_cast<std::string>(mChangedNamesCounter++);
	}
	if (str.find_first_of(' ') != std::string::npos) {
		std::replace(str.begin(), str.end(), ' ', '_');
	}
	if (usedNames.count(str) > 0) {
		str += "_" + boost::lexical_cast<std::string>(mChangedNamesCounter++);
		Logger::Warn("Renamed '" + str.substr(0, str.find_last_of("_")) + "' to '" + str + "' to avoid duplicate IDs");
	}
	usedNames.emplace(str, 'x');

	ai->Set(str);
}


