#include "mesh_exporter.h"
#include "data_converter.h"

XML3DMeshExporter::XML3DMeshExporter(XML3DExporter* ex, aiMesh* mesh) : 
	xml3d(ex),
	aMesh(mesh)
{

}

XML3DMeshExporter::~XML3DMeshExporter() {

}

tinyxml2::XMLElement* XML3DMeshExporter::getAssetMesh(aiMatrix4x4* parentTransform) {
	tinyxml2::XMLElement* mesh = xml3d->doc.NewElement("assetmesh");

	mesh->SetAttribute("includes", aMesh->mName.C_Str());
	if (xml3d->scene->HasMaterials()) {
		aiMaterial* mat = xml3d->scene->mMaterials[aMesh->mMaterialIndex];
		aiString name;
		mat->Get(AI_MATKEY_NAME, name);
		mat->AddProperty(&name, AI_MATKEY_NAME);
		xml3d->stringToHTMLId(name);
		std::string namestr("#");
		namestr.append(name.C_Str());
		mesh->SetAttribute("material", namestr.c_str());
	}
	mesh->SetAttribute("style", XML3DDataConverter::toXml3dString(*parentTransform).c_str());
	mesh->SetAttribute("type", "triangles");
	return mesh;
}

tinyxml2::XMLElement* XML3DMeshExporter::getAssetData() {
	tinyxml2::XMLElement* data = xml3d->doc.NewElement("assetdata");
	xml3d->stringToHTMLId(aMesh->mName);
	data->SetAttribute("name", aMesh->mName.C_Str());

	//Export indices
	tinyxml2::XMLElement* index = xml3d->doc.NewElement("int");
	index->SetAttribute("name", "index");
	index->SetText(XML3DDataConverter::toXml3dString(aMesh->mFaces, aMesh->mNumFaces).c_str());
	data->LinkEndChild(index);

	//Export positions
	tinyxml2::XMLElement* pos = xml3d->doc.NewElement("float3");
	pos->SetAttribute("name", "position");
	pos->SetText(XML3DDataConverter::toXml3dString(aMesh->mVertices, aMesh->mNumVertices).c_str());
	data->LinkEndChild(pos);

	//Export normals
	if (aMesh->HasNormals()) {
		tinyxml2::XMLElement* norm = xml3d->doc.NewElement("float3");
		norm->SetAttribute("name", "normal");
		norm->SetText(XML3DDataConverter::toXml3dString(aMesh->mNormals, aMesh->mNumVertices).c_str());
		data->LinkEndChild(norm);
	}

	//Export texcoords
	if (aMesh->GetNumUVChannels()) {
		tinyxml2::XMLElement* tc = xml3d->doc.NewElement("float2");
		tc->SetAttribute("name", "texcoord");
		// TODO: UV channel selection
		tc->SetText(XML3DDataConverter::toXml3dString(aMesh->mTextureCoords[0], aMesh->mNumVertices, true).c_str());
		data->LinkEndChild(tc);
	}

	//Export colors
	if (aMesh->GetNumColorChannels()) {
		tinyxml2::XMLElement* color = xml3d->doc.NewElement("float3");
		color->SetAttribute("name", "color");
		// TODO: Color channel selection
		color->SetText(XML3DDataConverter::toXml3dString(aMesh->mColors[0], aMesh->mNumVertices, true).c_str());
		data->LinkEndChild(color);
	}

	return data;
}

