#include "SkinnedEntity.h"

SkinnedEntity::SkinnedEntity(std::string key, std::string texPath)
	: VirtualEntity()
{
	mModelInstance = new SkinnedModel::Instance();

	mModelInstance->mModel = Model->GetSkinnedModel(key, texPath);
	mModelInstance->mTimePos = 0.0f;
	mModelInstance->mAnimName = "combinedAnim_1";
	mModelInstance->mAnimIndex = mModelInstance->mModel->GetSkinData()->GetAnimIndex(mModelInstance->mAnimName);
	mModelInstance->mFinalTransforms.resize(mModelInstance->mModel->GetSkinData()->GetBones().size());
	mModelInstance->mStart = 51+15;
	mModelInstance->mEnd = 51+15;
	mModelInstance->mPlayForward = true;
}

SkinnedEntity::~SkinnedEntity(void)
{
	SafeDelete(mModelInstance);
}

void SkinnedEntity::Initialize(XMFLOAT3 position, float scale)
{
	VirtualEntity::Initialize(position, scale);
}

void SkinnedEntity::Draw(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera)
{
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devCon->IASetInputLayout(InputLayouts::mPosNorTexTanSkin);

	D3DX11_TECHNIQUE_DESC techDesc = {};
	activeTech->GetDesc(&techDesc);

	Effects::NormalFX->SetView(camera->GetViewMatrix());
	Effects::NormalFX->SetProj(camera->GetProjMatrix());
	Effects::NormalFX->SetWorld(XMLoadFloat4x4(&mModelInstance->mWorld));
	Effects::NormalFX->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(&mModelInstance->mWorld)));
	Effects::NormalFX->SetBoneTransforms(&mModelInstance->mFinalTransforms[0], mModelInstance->mFinalTransforms.size());

	for(UINT i(0); i != techDesc.Passes; ++i)
	{
		for(UINT j(0); j != mModelInstance->mModel->GetMeshCount(); ++j)
		{
			UINT index = mModelInstance->mModel->GetMesh(j)->GetMaterialIndex();
			Effects::NormalFX->SetDiffuseMap(mModelInstance->mModel->GetDiffMapSRV(index));
			//Effects::NormalFX->SetNormalMap(mModelInstance->mModel->GetNormalMapSRV(index));
			Effects::NormalFX->SetMaterial(mModelInstance->mModel->GetMaterial(index));

			activeTech->GetPassByIndex(i)->Apply(0, devCon);
			mModelInstance->mModel->GetMesh(j)->Draw(devCon);
		}
	}
}

void SkinnedEntity::Update(float dt)
{
	mModelInstance->Update(dt);
}

void SkinnedEntity::CalcWorld()
{
	XMStoreFloat4x4(
		&mModelInstance->mWorld,
		XMLoadFloat4x4(&this->mModelScale) *
		XMLoadFloat4x4(&this->mModelRot) *
		XMLoadFloat4x4(&this->mModelOffset));
}
