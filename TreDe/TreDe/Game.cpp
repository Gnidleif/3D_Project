#include "Game.h"
#include <sstream>
using namespace std;

Game::Game()
	:
	mPlayer(new Player("Gnidleif", XMFLOAT3(50.0f, 50.0f, 50.0f))),
	mDirLightAmount(1), mPointLightAmount(2), mSpotLightAmount(2)
{
	this->mDirLights = new DirectionalLight[mDirLightAmount];
	this->mPointLights = new PointLight[mPointLightAmount];
	this->mSpotLights = new SpotLight[mSpotLightAmount];

	mDirLights[0] = DirectionalLight(
		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), // Ambient
		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f), // Diffuse
		XMFLOAT4(0.4f, 0.4f, 0.5f, 1.0f), // Specular
		XMFLOAT3(0.5f, 0.5f, 0.0f), 0.0f); // Direction/Padding

	mPointLights[0] = PointLight(
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), // Ambient
		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), // Diffuse
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), // Specular
		XMFLOAT3(0.0f, 10.0f, 100.0f), 50.0f, // Position/Range
		XMFLOAT3(1.0f, 1.0f, 1.0f), 0.0f); // Attenuation/Padding

	mPointLights[1] = PointLight(
		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f), // Ambient
		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), // Diffuse
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), // Specular
		XMFLOAT3(100.0f, 10.0f, 0.0f), 10.0f, // Position/Range
		XMFLOAT3(1.0f, 1.0f, 1.0f), 0.0f); // Attenuation/Padding

	mSpotLights[0] = SpotLight(
		XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f), // Ambient
		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f), // Diffuse
		XMFLOAT4(0.4f, 0.4f, 0.5f, 1.0f), // Specular
		XMFLOAT3(0.0f, 10.0f, 0.0f), 50.0f, // Position/Range
		XMFLOAT3(50.0f, 50.0f, 50.0f), 1.0f, // Direction/Spot
		XMFLOAT3(1.0f, 1.0f, 1.0f), 0.0f); // Attenuation/Padding

	mSpotLights[1] = SpotLight(
		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), // Ambient
		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f), // Diffuse
		XMFLOAT4(0.4f, 0.4f, 0.5f, 1.0f), // Specular
		XMFLOAT3(200.0f, 10.0f, 200.0f), 10.0f, // Position/Range
		XMFLOAT3(50.0f, 50.0f, 50.0f), 1.0f, // Direction/Spot
		XMFLOAT3(1.0f, 1.0f, 1.0f), 0.0f); // Attenuation/Padding
}

Game::~Game()
{
	SafeDelete(mPlayer);
	SafeDelete(mTerrain);
	for(auto& it(mPlatforms.begin()); it != mPlatforms.end(); ++it)
	{
		SafeDelete(*it);
	}
	//SafeDelete(mCharacter);
	SafeDelete(mSkyBox);

	delete[] mDirLights;
	delete[] mPointLights;
	delete[] mSpotLights;
}

void Game::Initialize(ID3D11Device* device)
{
	mTerrain = new TerrainEntity("../Data/Textures/heightmap.bmp");
	mTerrain->Initialize(XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f);

	//mCharacter = new SkinnedEntity("../Data/Models/Skinned/Character/Character.dae", "../Data/Models/Skinned/Character/");
	//mCharacter->Initialize(XMFLOAT3(150.0f, 20.0f, 100.0f), 1.0f);

	for(UINT i(0); i != 4; ++i)
	{
		stringstream ss;
		ss << i + 1;
		mPlatforms.push_back(new StaticEntity("../Data/Models/Static/Platform" + ss.str() + "/Platform" + ss.str() + ".obj", "../Data/Models/Static/Platform" + ss.str() + "/"));
		mPlatforms[i]->Initialize(XMFLOAT3(100.0f * (float)i, 20.0f, 100.0f), 0.05f);
	}

	mSkyBox = new SkyBox();
	mSkyBox->Initialize(device, 5000.0f);

	//
	Text->AddConstantText("PlayerInfo", "Name: " + mPlayer->GetName(), 20.0f, 20.0f, 20.0f, TextColors::White);
}

void Game::Update(float dt)
{
	Text->AddConstantText("CamRot", "Rotation: " + Text->XMFLOAT3ToString(mPlayer->GetCamera()->GetYawPitchRoll()), 20.0f, 50.0f, 20.0f, TextColors::Red);
	Text->AddConstantText("CamPos", "Position: " + Text->XMFLOAT3ToString(mPlayer->GetPosition()), 20.0f, 80.0f, 20.0f, TextColors::Red);

	static float xRot(0.0f), zRot(0.0f);
	xRot += dt;
	zRot += dt;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->RotateXYZ(XMFLOAT3(xRot, 0.0f, zRot));
	}

	//for(UINT i(0); i != mSpotLightAmount; ++i)
	//{
	//	mSpotLights[i].Position = mPlayer->GetCamera()->GetPosition();
	//	mSpotLights[i].Direction = mPlayer->GetCamera()->GetLook();
	//}

	mPointLights[0].Position = mPlayer->GetCamera()->GetPosition();
	//mSpotLights[0].Direction = mPlayer->GetCamera()->GetLook();

	//mCharacter->Update(dt);

	mPlayer->Update(dt);
}

void Game::SolidDraw(ID3D11DeviceContext* devCon, bool lightsOn)
{
	Camera* playerCam = mPlayer->GetCamera();

	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mSolid;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	if(lightsOn)
	{
		Effects::NormalFX->SetEyePos(&playerCam->GetPosition());
		Effects::NormalFX->SetDirLights(mDirLights, mDirLightAmount);
		Effects::NormalFX->SetPointLights(mPointLights, mPointLightAmount);
		Effects::NormalFX->SetSpotLights(mSpotLights, mSpotLightAmount);

		Effects::TerrainFX->SetEyePos(&playerCam->GetPosition());
		Effects::TerrainFX->SetDirLights(mDirLights, mDirLightAmount);
		Effects::TerrainFX->SetPointLights(mPointLights, mPointLightAmount);
		Effects::TerrainFX->SetSpotLights(mSpotLights, mSpotLightAmount);

		activeTech = Effects::TerrainFX->mAllLights;
		mTerrain->Draw(devCon, activeTech, playerCam);

		activeTech = Effects::NormalFX->mAllLights; // Add an alpha clip with lights later when the lights are fixed
		for(UINT i(0); i != mPlatforms.size(); ++i)
		{
			mPlatforms[i]->Draw(devCon, activeTech, playerCam);
		}
	}
	else
	{
		activeTech = Effects::TerrainFX->mSolid;
		mTerrain->Draw(devCon, activeTech, playerCam);

		activeTech = Effects::NormalFX->mSolidAlpha;
		for(UINT i(0); i != mPlatforms.size(); ++i)
		{
			mPlatforms[i]->Draw(devCon, activeTech, playerCam);
		}
	}

	//activeTech = Effects::NormalFX->mSolidSkin;
	//mCharacter->Draw(devCon, activeTech, playerCam);
}

void Game::WireDraw(ID3D11DeviceContext* devCon)
{
	Camera* playerCam = mPlayer->GetCamera();
	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mWire;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::TerrainFX->mWire;
	mTerrain->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::NormalFX->mWire;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->Draw(devCon, activeTech, playerCam);
	}
	//mCharacter->Draw(devCon, activeTech, playerCam);
}

void Game::ControlPlayer(DirectInput* di)
{
	if(di->GetKeyboardState()[DIK_R] && 0x80)
	{
		for(UINT i(0); i != mPlatforms.size(); ++i)
		{
			mPlatforms[i]->SetScale(0.05f);
		}
		//mCharacter->SetScale(0.5f);
	}
	else if(di->GetKeyboardState()[DIK_F] && 0x80)
	{
		for(UINT i(0); i != mPlatforms.size(); ++i)
		{
			mPlatforms[i]->SetScale(0.1f);
		}
		//mCharacter->SetScale(1.0f);
	}
	else
		mPlayer->Control(di);
}