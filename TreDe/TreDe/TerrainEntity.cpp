#include "TerrainEntity.h"
using namespace std;

TerrainEntity::TerrainEntity(string key)
	: VirtualEntity()
{
	mModelInstance.mModel = Model->GetTerrainModel(key);
}

TerrainEntity::~TerrainEntity(void)
{
}

void TerrainEntity::Initialize(XMFLOAT3 position, float scale)
{
	VirtualEntity::Initialize(position, scale);
}

void TerrainEntity::Draw(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera)
{
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devCon->IASetInputLayout(InputLayouts::mPosNorTex);

	D3DX11_TECHNIQUE_DESC techDesc = {};
	activeTech->GetDesc(&techDesc);

	Effects::TerrainFX->SetMaterial(mModelInstance.mModel->GetMaterial());
	Effects::TerrainFX->SetView(camera->GetViewMatrix());
	Effects::TerrainFX->SetProj(camera->GetProjMatrix());
	Effects::TerrainFX->SetWorld(XMLoadFloat4x4(&mModelInstance.mWorld));
	Effects::TerrainFX->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(&mModelInstance.mWorld)));

	Effects::TerrainFX->SetBlendMap(mModelInstance.mModel->GetBlendMap());
	Effects::TerrainFX->SetTex0(mModelInstance.mModel->GetTex0());
	Effects::TerrainFX->SetTex1(mModelInstance.mModel->GetTex1());
	Effects::TerrainFX->SetTex2(mModelInstance.mModel->GetTex2());
	Effects::TerrainFX->SetTex3(mModelInstance.mModel->GetTex3());

	for(UINT i(0); i != techDesc.Passes; ++i)
	{
		activeTech->GetPassByIndex(i)->Apply(0, devCon);
		mModelInstance.mModel->GetMesh()->Draw(devCon);
	}
}

void TerrainEntity::DrawTess(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera)
{
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	devCon->IASetInputLayout(InputLayouts::mPosNorTex);

	D3DX11_TECHNIQUE_DESC techDesc = {};
	activeTech->GetDesc(&techDesc);

	Effects::TerrTessFX->SetMaterial(mModelInstance.mModel->GetMaterial());
	Effects::TerrTessFX->SetView(camera->GetViewMatrix());
	Effects::TerrTessFX->SetProj(camera->GetProjMatrix());
	Effects::TerrTessFX->SetWorld(XMLoadFloat4x4(&mModelInstance.mWorld));
	Effects::TerrTessFX->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(&mModelInstance.mWorld)));

	Effects::TerrTessFX->SetBlendMap(mModelInstance.mModel->GetBlendMap());
	Effects::TerrTessFX->SetTex0(mModelInstance.mModel->GetTex0());
	Effects::TerrTessFX->SetTex1(mModelInstance.mModel->GetTex1());
	Effects::TerrTessFX->SetTex2(mModelInstance.mModel->GetTex2());
	Effects::TerrTessFX->SetTex3(mModelInstance.mModel->GetTex3());

	for(UINT i(0); i != techDesc.Passes; ++i)
	{
		activeTech->GetPassByIndex(i)->Apply(0, devCon);
		mModelInstance.mModel->GetMesh()->Draw(devCon);
	}
}

void TerrainEntity::CalcWorld()
{
	XMStoreFloat4x4(
		&mModelInstance.mWorld,
		XMLoadFloat4x4(&this->mModelScale) *
		XMLoadFloat4x4(&this->mModelRot) *
		XMLoadFloat4x4(&this->mModelOffset));
}