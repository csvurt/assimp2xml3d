
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/scene.h>
#include "tinyxml2.h"


class XML3DExporter {
public:
	XML3DExporter();
	~XML3DExporter();

	void writeFile(const char* file);
	void Export(const aiScene& ai);
	void Export(const aiMesh& am);
	void Export(const aiNode& an);
	void Export(const aiMaterial& amat);
	void Export(const aiTexture& at);
	void Export(const aiLight& al);

private:

	tinyxml2::XMLDocument doc;

};



