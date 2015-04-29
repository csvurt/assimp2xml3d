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

int invalidUsageExit() {
	std::cout << "usage: assimp2xml3d [FLAGS] inputFile [outputFile]" << std::endl;
	return -1;
}


int main(int argc, char *argv[]) {
	if (argc < 2) {
		return invalidUsageExit();
	}


}