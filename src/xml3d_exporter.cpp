
#include "mesh_exporter.h"
#include "material_exporter.h"
#include "logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
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
	doc.InsertFirstChild(doc.NewDeclaration());
	tinyxml2::XMLElement* xml3d = doc.NewElement("xml3d");
	tinyxml2::XMLElement* defs = doc.NewElement("defs");
	tinyxml2::XMLElement* asset = doc.NewElement("asset");
	xml3d->InsertFirstChild(defs);
	xml3d->LinkEndChild(asset);
	doc.LinkEndChild(xml3d);

	std::string id(filename);
	id = id.substr(0, id.find_first_of('.'));
	asset->SetAttribute("id", id.c_str());

	removeDummyMaterial(scene);

	if (scene->HasMeshes()) {
		for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
			XML3DMeshExporter mexp(this, scene->mMeshes[i]);
			tinyxml2::XMLElement* data = mexp.getAssetData();
			asset->InsertFirstChild(data);
		}
	}

	if (scene->HasMaterials()) {
		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
			XML3DMaterialExporter matExp(this, scene->mMaterials[i]);
			tinyxml2::XMLElement* material = matExp.getMaterial();
			defs->LinkEndChild(material);
			mNumberOfMaterialsExported++;
		}
	}

	// Flatten scene hierarchy into a list of assetmeshes
	Export(asset, scene->mRootNode, aiMatrix4x4());

	Logger::Info("Processed " + boost::lexical_cast<std::string>(mNumberOfMeshesExported) + " meshes and " +
		boost::lexical_cast<std::string>(mNumberOfMaterialsExported) + " materials.");
}

// Assimp will always generate a material even if it was instructed to ignore materials during the import process.
// To prevent this material from being exported when the --no-material flag was set we implicitly remove it from the scene 
// by setting the material count to 0.
void XML3DExporter::removeDummyMaterial(aiScene* scene) {
	if (scene->mNumMaterials == 1) {
		aiMaterial* mat = scene->mMaterials[0];
		aiString name;
		mat->Get(AI_MATKEY_NAME, name);
		if (!strcmp(name.C_Str(), "Dummy_MaterialsRemoved")) {
			scene->mNumMaterials = 0; //Will cause HasMaterials to return false from now on
		}
	}
}

void XML3DExporter::Export(tinyxml2::XMLElement* parent, aiNode* an, const aiMatrix4x4& parentTransform) {
	// Flatten all non-mesh nodes while gathering the transformations 

	aiMatrix4x4 t(an->mTransformation);
	t = parentTransform * t;

	for (unsigned int i = 0; i < an->mNumMeshes; i++) {
		XML3DMeshExporter mexp(this, scene->mMeshes[an->mMeshes[i]]);
		tinyxml2::XMLElement* mesh = mexp.getAssetMesh(&t);
		parent->LinkEndChild(mesh);
		mNumberOfMeshesExported++;
	}

	for (unsigned int i = 0; i < an->mNumChildren; i++) {
		Export(parent, an->mChildren[i], t);
	}
}


void XML3DExporter::writeFile() {
	doc.SaveFile(filename);
}


void XML3DExporter::stringToHTMLId(aiString& ai) {
	// Ensure the name is not empty and is safe to use as an HTML5 id string
	std::string str(ai.C_Str());

	if (!(str.length() > 0)) {
		str = "_Generated_Name_" + boost::lexical_cast<std::string>(mChangedNamesCounter++);
	}

	std::replace(str.begin(), str.end(), ' ', '_');

	if (usedNames.count(str) > 0) {
		str += "_" + boost::lexical_cast<std::string>(mChangedNamesCounter++);
		Logger::Warn("Renamed '" + str.substr(0, str.find_last_of("_")) + "' to '" + str + "' to avoid duplicate IDs");
	}
	usedNames.emplace(str, 'x');

	ai.Set(str);
}


