
#include "xml3d_exporter.h"
#include <iostream>

namespace {
	void ExportScene(const char*, Assimp::IOSystem*, const aiScene*, const Assimp::ExportProperties*);
}


Assimp::Exporter::ExportFormatEntry Assimp2XML3D_desc = Assimp::Exporter::ExportFormatEntry(
	"xml",
	"XML representation of the scene, compatible for use with XML3D as an external resource.",
	"xml",
	ExportScene,
	0u);

namespace {
	void ExportScene(const char* file, Assimp::IOSystem* out, const aiScene* scene, const Assimp::ExportProperties*) {
		XML3DExporter exp;
		exp.Export(*scene);
	}
}

XML3DExporter::XML3DExporter() {

}
XML3DExporter::~XML3DExporter() {
	doc.Clear();
}

void XML3DExporter::Export(const aiScene& ai) {

}

void XML3DExporter::Export(const aiMesh& am) {

}

void XML3DExporter::Export(const aiNode& an) {

}

void XML3DExporter::Export(const aiMaterial& amat) {

}

void XML3DExporter::Export(const aiTexture& at) {

}

void XML3DExporter::Export(const aiLight& al) {

}


