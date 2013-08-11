#include "LightHandler.h"
#include "Effects.h"
#include "InputLayouts.h"

LightHandler::LightHandler(void)
	: 
	mShadowMap(new ShadowMap()),
	mDirAmount(1),
	mPointAmount(2),
	mSpotAmount(2)
{
	mDirs = new DirectionalLight[mDirAmount];
	mPoints = new PointLight[mPointAmount];
	mSpots = new SpotLight[mSpotAmount];

	// Directional lights initialized

	mDirs[0].Ambient	= XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirs[0].Diffuse	= XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	mDirs[0].Specular	= XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirs[0].Direction	= XMFLOAT3(-0.5f, -0.5f, 0.5f);

	// Pointlights initialized

	mPoints[0].Ambient		= XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mPoints[0].Diffuse		= XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPoints[0].Specular		= XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPoints[0].Attenuation	= XMFLOAT3(0.0f, 0.1f, 0.0f);
	mPoints[0].Range		= 750.0f;

	mPoints[1].Ambient		= XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mPoints[1].Diffuse		= XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPoints[1].Specular		= XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mPoints[1].Attenuation  = XMFLOAT3(1.0f, 1.0f, 1.0f);
	mPoints[1].Range		= 20000.0f;
	mPoints[1].Position		= XMFLOAT3(400.0f, 600.0f, 500.0f);

	// Spotlights initialized

	mSpots[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mSpots[0].Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mSpots[0].Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mSpots[0].Range = 75.0f;
	mSpots[0].Position = XMFLOAT3(0.0f, 25.0f, 150.0f);
	mSpots[0].Direction = XMFLOAT3(-0.5f, -0.5f, 0.5f);

	mSpots[1].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mSpots[1].Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mSpots[1].Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mSpots[1].Range = 75.0f;
	mSpots[1].Position = XMFLOAT3(150.0f, 25.0f, 0.0f);
	mSpots[1].Direction = XMFLOAT3(-0.5f, -0.5f, 0.5f);
}

LightHandler::~LightHandler(void)
{
	delete[] mDirs;
	delete[] mPoints;
	delete[] mSpots;
	SafeDelete(mShadowMap);
}

void LightHandler::Initialize(ID3D11Device* device)
{
	this->mShadowMap->Initialize(device);
}

void LightHandler::Update(float dt)
{
	static float time = 0.0f;
	time += dt;
	float x = 250.0f*cosf(0.3f*time) + 1000.0f;
	float z = 750.0f*sinf(0.3f*time) + 1000.0f;
	float y = 500.0f;

	mPoints[0].Position = XMFLOAT3(x, y, z);
}

void LightHandler::Draw(std::vector<StaticEntity*> statics, TerrainEntity* terrain, ID3D11DeviceContext* devCon, Camera* camera)
{
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devCon->IASetInputLayout(InputLayouts::mPos);
	
	this->mShadowMap->BindDSVAndSetRTV(devCon);
	this->mShadowMap->ResetMap();

	Effects::TerrainFX->SetShadowMap(this->mShadowMap->GetDepthSRV());
	Effects::NormalFX->SetShadowMap(this->mShadowMap->GetDepthSRV());

	XMMATRIX lightVP = XMMatrixMultiply(this->CalcView(camera), this->CalcProj(camera));

	Effects::TerrainFX->SetLightVP(lightVP);
	Effects::NormalFX->SetLightVP(lightVP);
	Effects::ShadowFX->SetLightVP(lightVP);

	D3DX11_TECHNIQUE_DESC techDesc = {};
	Effects::ShadowFX->mShadowTech->GetDesc(&techDesc);

	for(UINT i(0); i != techDesc.Passes; ++i)
	{
		Effects::ShadowFX->SetWorld(XMLoadFloat4x4(&terrain->GetInstance()->mWorld));

		Effects::ShadowFX->mShadowTech->GetPassByIndex(i)->Apply(0, devCon);
		terrain->GetInstance()->mModel->GetMesh()->Draw(devCon);
	}

	for(UINT i(0); i != techDesc.Passes; ++i)
	{
		for(UINT j(0); j != statics.size(); ++j)
		{
			Effects::ShadowFX->SetWorld(XMLoadFloat4x4(&statics[j]->GetInstance()->mWorld));

			for(UINT k(0); k != statics[j]->GetInstance()->mModel->GetMeshCount(); ++k)
			{
				Effects::ShadowFX->mShadowTech->GetPassByIndex(i)->Apply(0, devCon);
				statics[j]->GetInstance()->mModel->GetMesh(k)->Draw(devCon);
			}
		}
	}
}

void LightHandler::ApplyEffects()
{
	Effects::TerrainFX->SetDirLights(mDirs, mDirAmount);
	Effects::TerrainFX->SetPointLights(mPoints, mPointAmount);
	Effects::TerrainFX->SetSpotLights(mSpots, mSpotAmount);

	Effects::NormalFX->SetDirLights(mDirs, mDirAmount);
	Effects::NormalFX->SetPointLights(mPoints, mPointAmount);
	Effects::NormalFX->SetSpotLights(mSpots, mSpotAmount);
}

void LightHandler::ApplyTessEffects()
{
	Effects::TessFX->SetDirLights(mDirs, mDirAmount);
	Effects::TessFX->SetPointLights(mPoints, mPointAmount);
	Effects::TessFX->SetSpotLights(mSpots, mSpotAmount);

	Effects::TerrTessFX->SetDirLights(mDirs, mDirAmount);
	Effects::TerrTessFX->SetPointLights(mPoints, mPointAmount);
	Effects::TerrTessFX->SetSpotLights(mSpots, mSpotAmount);
}

XMMATRIX LightHandler::CalcView(Camera* camera)
{
	XMVECTOR posVec = XMLoadFloat3(&mPoints[0].Position);
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));
	XMVECTOR look = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 1.0f));
	return XMMatrixLookAtLH(posVec, look, up);
}

XMMATRIX LightHandler::CalcProj(Camera* camera)
{
	return XMMatrixPerspectiveFovLH(camera->GetFovY(), camera->GetAspect(), camera->GetNearZ(), camera->GetFarZ());
}
