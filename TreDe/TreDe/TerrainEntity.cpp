#include "TerrainEntity.h"
using namespace std;

TerrainEntity::TerrainEntity(string key)
	: VirtualEntity()
{
	mModelInstance.mModel = Model->GetTerrainModel(key);
	Effects::TerrainFX->SetMaterial(mModelInstance.mModel->GetMaterial());
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

	Effects::TerrainFX->SetView(&camera->GetViewMatrix());
	Effects::TerrainFX->SetProj(&camera->GetProjMatrix());
	this->mModelInstance.mModel->ApplyEffects();

	for(UINT i(0); i != techDesc.Passes; ++i)
	{
		activeTech->GetPassByIndex(i)->Apply(0, devCon);
	}
	mModelInstance.mModel->GetMesh()->Draw(devCon);
}

void TerrainEntity::CalcWorld()
{
	XMStoreFloat4x4(
		&mModelInstance.mWorld,
		XMLoadFloat4x4(&this->mModelScale) *
		XMLoadFloat4x4(&this->mModelRot) *
		XMLoadFloat4x4(&this->mModelOffset));
	Effects::TerrainFX->SetWorld(&XMLoadFloat4x4(&mModelInstance.mWorld));
	Effects::TerrainFX->SetWorldInvTranspose(&MathHelper::InverseTranspose(XMLoadFloat4x4(&mModelInstance.mWorld)));
}