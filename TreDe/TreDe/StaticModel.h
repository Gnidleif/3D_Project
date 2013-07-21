#ifndef STATICMODEL_H // Finish later
#define STATICMODEL_H
#include "VirtualModel.h"
#include "Meshes.h"
#include "LightDef.h"
#include "LoaderClass.h"

class StaticModel : public VirtualModel
{
public:
	StaticModel(ID3D11Device* device, std::string filename, std::string texPath);
	~StaticModel(void);
	void ApplyEffects();

public:
	StaticMesh* GetMesh(UINT index) const { return this->mMeshes[index]; }

private:
	void CreateMatsAndMeshes(std::string filename);
	void CreateBuffers();

private:
	std::vector<Material> mMaterials;
	std::vector<StaticMesh*> mMeshes;
	std::vector<ID3D11ShaderResourceView*> mDiffMapSRV;
	std::vector<ID3D11ShaderResourceView*> mNormalMapSRV;
	std::vector<GenericMaterial> mGenMats;

public:
	struct Instance
	{
		StaticModel* mModel;
		XMFLOAT4X4 mWorld;
	};
};
#endif
