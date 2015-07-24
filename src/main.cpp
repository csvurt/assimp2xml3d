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

struct Environment {
	Logger::Level	logLevel = Logger::WARN;
	std::string		inputFile = "";
	std::string		outputFile = "";
	unsigned int	importFlags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_RemoveComponent;
	unsigned int	ignoreComponents = aiComponent_LIGHTS | aiComponent_CAMERAS;
};

int invalidUsageExit() {
	std::cout << "usage: assimp2xml3d [--verbose|--no-materials] inputFile outputFile" << std::endl;
	return -1;
}

void printHeader() {
	std::cout << " ______ assimp2xml3d v" << ASSIMP2XML3D_VERSION_MAJOR << "." << ASSIMP2XML3D_VERSION_MINOR << "." << ASSIMP2XML3D_VERSION_PATCH;
	std::cout << "  (using assimp v"
		<< aiGetVersionMajor() << "."
		<< aiGetVersionMinor() << "."
		<< aiGetVersionRevision() << ") ______"
		<< std::endl << std::endl;
}


bool parseArgs(int argc, char *argv[], Environment &env) {
	int nextarg = 1;
	while (nextarg < argc && argv[nextarg][0] == '-') {
		if (!strcmp(argv[nextarg], "--verbose") || !strcmp(argv[nextarg], "-v")) {
			env.logLevel = Logger::INFO;
		}
		else if (!strcmp(argv[nextarg], "--no-materials") || !strcmp(argv[nextarg], "-m")) {
			env.ignoreComponents |= aiComponent_MATERIALS;
		}
		++nextarg;
	}

	if (nextarg + 1 >= argc) {
		// Missing input or output file
		return false;
	}

	env.inputFile = std::string(argv[nextarg]);
	env.outputFile = std::string(argv[++nextarg]);

	return true;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		return invalidUsageExit();
	}
	Environment env;

	if (!parseArgs(argc, argv, env)) {
		return invalidUsageExit();
	}

	printHeader();

	Logger::logLevel = env.logLevel;

	Logger::Info("Beginning Assimp import of " + env.inputFile);

	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true);
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, env.ignoreComponents);
	Logger::Info("Instructing assimp to remove degenerate triangles and ignore points/lines.");

	const aiScene* const scene = importer.ReadFile(env.inputFile.c_str(), env.importFlags);

	if (!scene) {
		Logger::Error("Assimp: Could not read file " + env.inputFile);
		Logger::Error(importer.GetErrorString());
		return -1;
	}

	Logger::Info("Assimp import of " + env.inputFile + " was successful. Beginning export to XML3D.");

	Assimp::Exporter exporter;
	exporter.RegisterExporter(Assimp2XML3D_desc);
	exporter.Export(scene, "xml", env.outputFile.c_str());

	Logger::Info("Finished exporting to XML3D, all systems nominal.");
}
