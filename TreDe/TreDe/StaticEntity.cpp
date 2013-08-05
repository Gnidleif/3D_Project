#include "StaticEntity.h"

StaticEntity::StaticEntity(std::string key, std::string texPath)
	: VirtualEntity()
{
	mModelInstance.mModel = Model->GetStaticModel(key, texPath);
}

StaticEntity::~StaticEntity(void)
{
}

void StaticEntity::Initialize(XMFLOAT3 position, float scale)
{
	VirtualEntity::Initialize(position, scale);
}

void StaticEntity::Draw(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera)
{
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devCon->IASetInputLayout(InputLayouts::mPosNorTexTan);

	D3DX11_TECHNIQUE_DESC techDesc = {};
	activeTech->GetDesc(&techDesc);

	Effects::NormalFX->SetView(camera->GetViewMatrix());
	Effects::NormalFX->SetProj(camera->GetProjMatrix());
	Effects::NormalFX->SetWorld(XMLoadFloat4x4(&mModelInstance.mWorld));
	Effects::NormalFX->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(&mModelInstance.mWorld)));

	for(UINT i(0); i != techDesc.Passes; ++i)
	{
		for(UINT j(0); j != mModelInstance.mModel->GetMeshCount(); ++j)
		{
			UINT index = mModelInstance.mModel->GetMesh(j)->GetMaterialIndex();
			Effects::NormalFX->SetDiffuseMap(mModelInstance.mModel->GetDiffMapSRV(index));
			Effects::NormalFX->SetNormalMap(mModelInstance.mModel->GetNormalMapSRV(index));
			Effects::NormalFX->SetMaterial(mModelInstance.mModel->GetMaterial(index));

			activeTech->GetPassByIndex(i)->Apply(0, devCon);
			mModelInstance.mModel->GetMesh(j)->Draw(devCon);
		}
	}
}

void StaticEntity::DrawTess(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera)
{
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	devCon->IASetInputLayout(InputLayouts::mPosNorTexTan);

	D3DX11_TECHNIQUE_DESC techDesc = {};
	activeTech->GetDesc(&techDesc);
	Effects::ShadowFX->SetWorld(XMLoadFloat4x4(&mModelInstance.mWorld));

	Effects::TessFX->SetView(camera->GetViewMatrix());
	Effects::TessFX->SetProj(camera->GetProjMatrix());
	Effects::TessFX->SetWorld(XMLoadFloat4x4(&mModelInstance.mWorld));
	Effects::TessFX->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(&mModelInstance.mWorld)));

	for(UINT i(0); i != techDesc.Passes; ++i)
	{
		for(UINT j(0); j != mModelInstance.mModel->GetMeshCount(); ++j)
		{
			UINT index = mModelInstance.mModel->GetMesh(j)->GetMaterialIndex();
			Effects::TessFX->SetDiffuseMap(mModelInstance.mModel->GetDiffMapSRV(index));
			Effects::TessFX->SetNormalMap(mModelInstance.mModel->GetNormalMapSRV(index));
			Effects::TessFX->SetMaterial(mModelInstance.mModel->GetMaterial(index));

			activeTech->GetPassByIndex(i)->Apply(0, devCon);
			mModelInstance.mModel->GetMesh(j)->Draw(devCon);
		}
	}
}

void StaticEntity::CalcWorld()
{
	XMStoreFloat4x4(
		&mModelInstance.mWorld,
		XMLoadFloat4x4(&this->mModelScale) *
		XMLoadFloat4x4(&this->mModelRot) *
		XMLoadFloat4x4(&this->mModelOffset));
}
