#include "Game.h"
#include <sstream>
using namespace std;

Game::Game()
	:
	mPlayer(new Player("Gnidleif", XMFLOAT3(50.0f, 50.0f, 50.0f))),
	mLightHandler(new LightHandler())
{
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
	SafeDelete(mLightHandler);
	for(auto& it(mLightDucks.begin()); it != mLightDucks.end(); ++it)
	{
		SafeDelete(*it);
	}
}

void Game::Initialize(ID3D11Device* device)
{
	mTerrain = new TerrainEntity("../Data/Textures/heightmap.bmp");
	mTerrain->Initialize(XMFLOAT3(0.0f, 0.0f, 0.0f), 5.0f);

	//mCharacter = new SkinnedEntity("../Data/Models/Skinned/Character/Character.dae", "../Data/Models/Skinned/Character/");
	//mCharacter->Initialize(XMFLOAT3(150.0f, 20.0f, 100.0f), 1.0f);

	for(UINT i(0); i != 4; ++i)
	{
		stringstream ss;
		ss << i + 1;
		mPlatforms.push_back(new StaticEntity("../Data/Models/Static/Platform" + ss.str() + "/Platform" + ss.str() + ".obj", "../Data/Models/Static/Platform" + ss.str() + "/"));
		mPlatforms[i]->Initialize(XMFLOAT3(400.0f * (float)i, 500.0f, 100.0f), 0.5f);
	}

	mSkyBox = new SkyBox();
	mSkyBox->Initialize(device, 5000.0f);

	mLightDucks.push_back(new StaticEntity("../Data/Models/Static/Duck/Duck.obj", "../Data/Models/Static/Duck/"));
	mLightDucks.push_back(new StaticEntity("../Data/Models/Static/Duck/Duck.obj", "../Data/Models/Static/Duck/"));

	XMFLOAT3 duckPos = mLightHandler->GetPoint0Pos();
	mLightDucks[0]->Initialize(duckPos, 0.05f);

	duckPos = mLightHandler->GetPoint1Pos();
	mLightDucks[1]->Initialize(duckPos, 0.05f);

	//
	Text->AddConstantText("PlayerInfo", "Name: " + mPlayer->GetName(), 20.0f, 20.0f, 20.0f, TextColors::White);
}

void Game::Update(float dt)
{
	Text->AddConstantText("CamRot", "Rotation: " + Text->XMFLOAT3ToString(mPlayer->GetCamera()->GetYawPitchRoll()), 20.0f, 50.0f, 20.0f, TextColors::Red);
	Text->AddConstantText("CamPos", "Position: " + Text->XMFLOAT3ToString(mPlayer->GetPosition()), 20.0f, 80.0f, 20.0f, TextColors::Red);

	static float rot(0.0f);
	rot += dt;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->RotateXYZ(XMFLOAT3(rot, 0.0f, rot));
	}

	mLightDucks[0]->SetPosition(mLightHandler->GetPoint0Pos());
	mLightDucks[1]->SetPosition(mLightHandler->GetPoint1Pos());

	//mCharacter->Update(dt);

	mPlayer->Update(dt);
	mLightHandler->Update(dt);
}

void Game::SolidDraw(ID3D11DeviceContext* devCon)
{
	Camera* playerCam = mPlayer->GetCamera();

	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mSolid;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::TerrainFX->mSolid;
	mTerrain->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::NormalFX->mSolidAlpha;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->Draw(devCon, activeTech, playerCam);
	}
	for(UINT i(0); i != mLightDucks.size(); ++i)
	{
		mLightDucks[i]->Draw(devCon, activeTech, playerCam);
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
	for(UINT i(0); i != mLightDucks.size(); ++i)
	{
		mLightDucks[i]->Draw(devCon, activeTech, playerCam);
	}
	//mCharacter->Draw(devCon, activeTech, playerCam);
}

void Game::LightDraw(ID3D11DeviceContext* devCon)
{
	Camera* playerCam = mPlayer->GetCamera();

	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mSolid;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	mLightHandler->ApplyEffects();

	Effects::TerrainFX->SetEyePos(playerCam->GetPosition());
	Effects::NormalFX->SetEyePos(playerCam->GetPosition());

	activeTech = Effects::TerrainFX->mAllLights;
	mTerrain->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::NormalFX->mAllLightsAlpha;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->Draw(devCon, activeTech, playerCam);
	}
	for(UINT i(0); i != mLightDucks.size(); ++i)
	{
		mLightDucks[i]->Draw(devCon, activeTech, playerCam);
	}
}

// Tessellation draw methods

void Game::SolidTessDraw(ID3D11DeviceContext* devCon)
{
	Camera* playerCam = mPlayer->GetCamera();

	Effects::TessFX->SetMinTessDist(1.0f);
	Effects::TessFX->SetMaxTessDist(1024.0f);
	Effects::TessFX->SetMinTessFact(1.0f);
	Effects::TessFX->SetMaxTessFact(64.0f);
	Effects::TessFX->SetEyePos(playerCam->GetPosition());

	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mSolid;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::TerrainFX->mSolid;
	mTerrain->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::TessFX->mSolidAlpha;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->DrawTess(devCon, activeTech, playerCam);
	}
	for(UINT i(0); i != mLightDucks.size(); ++i)
	{
		mLightDucks[i]->DrawTess(devCon, activeTech, playerCam);
	}
}

void Game::WireTessDraw(ID3D11DeviceContext* devCon)
{
	Camera* playerCam = mPlayer->GetCamera();

	Effects::TessFX->SetMinTessDist(1.0f);
	Effects::TessFX->SetMaxTessDist(1024.0f);
	Effects::TessFX->SetMinTessFact(1.0f);
	Effects::TessFX->SetMaxTessFact(64.0f);
	Effects::TessFX->SetEyePos(playerCam->GetPosition());

	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mWire;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::TerrainFX->mWire;
	mTerrain->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::TessFX->mWire;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->DrawTess(devCon, activeTech, playerCam);
	}
	for(UINT i(0); i != mLightDucks.size(); ++i)
	{
		mLightDucks[i]->DrawTess(devCon, activeTech, playerCam);
	}
}

void Game::LightTessDraw(ID3D11DeviceContext* devCon)
{
	Camera* playerCam = mPlayer->GetCamera();

	Effects::TerrainFX->SetEyePos(playerCam->GetPosition());
	Effects::TessFX->SetMinTessDist(1.0f);
	Effects::TessFX->SetMaxTessDist(1024.0f);
	Effects::TessFX->SetMinTessFact(1.0f);
	Effects::TessFX->SetMaxTessFact(64.0f);
	Effects::TessFX->SetEyePos(playerCam->GetPosition());

	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mSolid;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	mLightHandler->ApplyEffects();

	activeTech = Effects::TerrainFX->mAllLights;
	mTerrain->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::TessFX->mAllLights;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->DrawTess(devCon, activeTech, playerCam);
	}
	for(UINT i(0); i != mLightDucks.size(); ++i)
	{
		mLightDucks[i]->DrawTess(devCon, activeTech, playerCam);
	}
}

void Game::ControlPlayer(DirectInput* di)
{
	mPlayer->Control(di);
}