#ifndef TERRAINMODEL_H
#define TERRAINMODEL_H
#include "VirtualModel.h"
#include "Meshes.h"
#include "LightDef.h"
#include "LoaderClass.h"
#include "Effects.h"

class TerrainModel : public VirtualModel
{
public:
	TerrainModel(std::string filename);
	~TerrainModel();

public:
	TerrainMesh* GetMesh() const { return this->mMesh; }
	Material* GetMaterial() const { return this->mMaterial; }

private:
	void CreateMatsAndMeshes(std::string filename);

private:
	TerrainMesh* mMesh;
	Material* mMaterial;
	ID3D11ShaderResourceView* mBlendMap;
	ID3D11ShaderResourceView* mTex0;
	ID3D11ShaderResourceView* mTex1;
	ID3D11ShaderResourceView* mTex2;
	ID3D11ShaderResourceView* mTex3;

public:
	struct Instance
	{
		TerrainModel* mModel;
		XMFLOAT4X4 mWorld;
	};
};
#endif