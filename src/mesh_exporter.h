#pragma once

#include "xml3d_exporter.h"

class XML3DMeshExporter {

public:
	XML3DMeshExporter(XML3DExporter* ex, aiMesh* mesh);
	~XML3DMeshExporter();

	tinyxml2::XMLElement* getAssetMesh(aiMatrix4x4* parentTransform);
	tinyxml2::XMLElement* getAssetData();

private:
	XML3DExporter* xml3d;
	aiMesh* aMesh;
};
