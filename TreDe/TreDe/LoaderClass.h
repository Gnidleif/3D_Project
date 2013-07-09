#ifndef LOADERCLASS_H
#define LOADERCLASS_H
#include "d3dUtilities.h"
#include "Meshes.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>

#define Loader LoaderClass::GetInstance()

// A struct used to hold various material-related variables,
// only used together with objectloading using Assimp, since Assimp requires material-stuff
struct GenericMaterial
{
	Material mMaterial;
	std::string mDiffMapName;
	std::string mNormalMapName;
	std::string mName;

	GenericMaterial()
	{
		ZeroMemory(&mMaterial, sizeof(mMaterial));
		this->mMaterial.Ambient		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		this->mMaterial.Diffuse		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		this->mMaterial.Specular	= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		this->mMaterial.Reflect		= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		this->mDiffMapName = "";
		this->mNormalMapName = "";
		this->mName = "";
	}
};
// This class loads 3D-objects for the ModelClass-class
class LoaderClass
{
public:
	// Yes, it's another Singleton, whatever, I do what I want
	static LoaderClass* GetInstance();
	void Shutdown();
	// The device is needed to set the vertex/indexbuffers of the meshes that this class creates
	void Initialize(ID3D11Device* device);
	// Function used to load the heightmap
	void CreateHeightMap(
		TerrainMesh* &mesh,
		std::string fileName,
		float normFactor,
		int texRepeat);

	bool CreateStaticObject(
		std::vector<GenericMaterial>& materials,
		std::vector<StaticMesh*>& meshes,
		std::string filename);

private:
	static LoaderClass* mInstance;
	ID3D11Device* mDevice;

private:
	// Terrain functions
	void LoadHeightMapFile(std::string filename);
	void NormalizeY(float factor);
	void CalcNormals();
	void CalcTexCoords(int repeat);

	// Static mesh functions
	void ReadMaterials(const aiScene* scene, std::vector<GenericMaterial>& materials);

	// Shared functions

private:
	// Terrain members
	UINT mRows, mCols;
	std::vector<VertexDef::PosNorTex> mHeightMap;

private: // Constructor/Destructor
	LoaderClass();
	~LoaderClass() {}
};
#endif