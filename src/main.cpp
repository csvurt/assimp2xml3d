/**
* Assimp2XML3D
*
* Copyright (c)2015, Christian Schlinkmann
*
*
**/

#include <assimp/version.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>

#include <iostream>

extern Assimp::Exporter::ExportFormatEntry Assimp2XML3D_desc;

int invalidUsageExit() {
	std::cout << "usage: assimp2xml3d [FLAGS] inputFile [outputFile]" << std::endl;
	return -1;
}


int main(int argc, char *argv[]) {
	if (argc < 3) {
		return invalidUsageExit();
	}

	const char* input = argv[1], *output = argv[2];

	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true);

	const aiScene* const scene = importer.ReadFile(input, aiProcessPreset_TargetRealtime_MaxQuality);

	if (!scene) {
		std::cerr << "Assimp: Could not read file " << input << std::endl;
		return -1;
	}

	std::cout << "Assimp read file successfully";

	Assimp::Exporter exporter;
	exporter.RegisterExporter(Assimp2XML3D_desc);
	exporter.Export(scene, "xml", output);
}
