#pragma once

#include "xml3d_exporter.h"

class XML3DMeshExporter {

public:
	XML3DMeshExporter();
	XML3DMeshExporter(XML3DExporter* ex, aiMesh* mesh);
	~XML3DMeshExporter();

	aiMesh* aMesh;
	tinyxml2::XMLElement* mDataElement;

	tinyxml2::XMLElement* getAssetMesh(aiMatrix4x4* parentTransform);
	tinyxml2::XMLElement* getAssetData();
	bool hasBones();

private:
	XML3DExporter* xml3d;
	

	void discoverBones();
};
