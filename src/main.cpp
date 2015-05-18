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
#include "logger.h"
#include "version.h"

#include <iostream>

extern Assimp::Exporter::ExportFormatEntry Assimp2XML3D_desc;

int invalidUsageExit() {
	std::cout << "usage: assimp2xml3d [-v|--verbose] inputFile outputFile" << std::endl;
	return -1;
}

void printHeader() {
	std::cout << " ______ assimp2xml3d v" << ASSIMP2XML3D_VERSION_MAJOR << "." << ASSIMP2XML3D_VERSION_MINOR;
	std::cout << "  (using assimp v"
		<< aiGetVersionMajor() << "."
		<< aiGetVersionMinor() << "."
		<< aiGetVersionRevision() << ") ______"
		<< std::endl << std::endl;
}


int main(int argc, char *argv[]) {
	if (argc < 3) {
		return invalidUsageExit();
	}

	int nextarg = 1;
	while (nextarg < argc && argv[nextarg][0] == '-') {
		if (!strcmp(argv[nextarg], "--verbose") || !strcmp(argv[nextarg], "-v")) {
			Logger::logLevel = Logger::INFO;
		}
		++nextarg;
	}

	if (nextarg + 1 >= argc) {
		return invalidUsageExit();
	}

	const char* input = argv[nextarg], *output = argv[++nextarg];

	printHeader();

	Logger::Info("Beginning Assimp import of " + std::string(input));

	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true);
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 1<<16);
	Logger::Info("Instructing assimp to remove degenerate triangles, ignore points/lines and split large meshes.");

	const aiScene* const scene = importer.ReadFile(input, aiProcessPreset_TargetRealtime_MaxQuality);

	if (!scene) {
		Logger::Error("Assimp: Could not read file " + std::string(input));
		Logger::Error(importer.GetErrorString());
		return -1;
	}

	Logger::Info("Assimp import of " + std::string(input) + " was successful. Beginning export to XML3D.");

	Assimp::Exporter exporter;
	exporter.RegisterExporter(Assimp2XML3D_desc);
	exporter.Export(scene, "xml", output);

	Logger::Info("Finished exporting to XML3D, all systems nominal.");
}
