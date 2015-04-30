
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/scene.h>
#include "tinyxml2.h"


class XML3DExporter {
public:
	XML3DExporter(const aiScene* scene, const char* filename);
	~XML3DExporter();

	void writeFile();
	void Export();
	void Export(tinyxml2::XMLElement* parent, aiMesh* am, aiMatrix4x4* parentTransform);
	void Export(tinyxml2::XMLElement* defs, aiMesh* am);
	void Export(tinyxml2::XMLElement* parent, aiNode* an, aiMatrix4x4* parentTransform);
	void Export(tinyxml2::XMLElement* parent, aiMaterial* amat);
	void Export(tinyxml2::XMLElement* parent, aiTexture* at);

private:
	aiScene* scene;
	const char* filename;
	unsigned int mUnnamedMeshIndex = 0;
	unsigned int mUnnamedMaterialIndex = 0;

	tinyxml2::XMLDocument doc;

	std::string toXml3dString(aiMatrix4x4* m);
	std::string toXml3dString(aiVector3D* v, unsigned int len, bool toVec2 = false);
	std::string toXml3dString(aiColor4D* v, unsigned int len, bool toVec3 = false);
	std::string toXml3dString(aiFace* f, unsigned int len);
	void stringToHTMLId(aiString* str, unsigned int& counter);


};



