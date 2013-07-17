#ifndef STATICMODEL_H // Finish later
#define STATICMODEL_H
#include "VirtualModel.h"
#include "Meshes.h"
#include "LightDef.h"
#include "LoaderClass.h"

class StaticModel : public VirtualModel
{
public:
	StaticModel(std::string filename, std::string texPath);
	~StaticModel(void);
	void ApplyEffects();

public:
	std::vector<StaticMesh*> GetMeshes() const { return this->mMeshes; }
	StaticMesh* GetMesh(UINT index) const { return this->mMeshes[index]; }
	std::vector<Material> GetMaterial() const { return this->mMaterials; }
	std::vector<ID3D11ShaderResourceView*> GetDiffMap() const { return this->mDiffMapSRV; }
	std::vector<ID3D11ShaderResourceView*> GetNormalMap() const { return this->mNormalMapSRV; }

private:
	void CreateMatsAndMeshes(std::string filename);

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
