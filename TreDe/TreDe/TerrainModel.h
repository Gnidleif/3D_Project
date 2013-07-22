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
	TerrainModel(ID3D11Device* device, std::string filename);
	~TerrainModel();
	void ApplyEffects();

public:
	TerrainMesh* GetMesh() const { return this->mMesh; }

private:
	void CreateMatsAndMeshes(std::string filename);
	void CreateBuffers();

private:
	TerrainMesh* mMesh;
	Material mMaterial;
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