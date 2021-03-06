#include "Game.h"
#include <sstream>
using namespace std;

Game::Game()
	:
	mPlayer(new Player("Gnidleif", XMFLOAT3(50.0f, 50.0f, 50.0f)))
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
	SafeDelete(mSkyBox);
}

void Game::Initialize(ID3D11Device* device)
{
	mTerrain = new TerrainEntity("../Data/Textures/heightmap.bmp");
	mTerrain->Initialize(XMFLOAT3(0.0f, 0.0f, 0.0f), 5.0f);

	mSkyBox = new SkyBox("../Data/Textures/SkyBox_Space.dds");
	mSkyBox->Initialize(device, 5000.0f);
	
	for(UINT i(0); i != 3; ++i)
	{
		stringstream ss;
		ss << i + 1;
		mPlatforms.push_back(new StaticEntity("../Data/Models/Static/Platform" + ss.str() + "/Platform" + ss.str() + ".obj", "../Data/Models/Static/Platform" + ss.str() + "/"));
		mPlatforms[i]->Initialize(XMFLOAT3(400.0f * (float)i, 300.0f, 400.0f), 0.5f);
	}
	
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

	mPlayer->Update(dt);
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

void Game::LightDraw(ID3D11DeviceContext* devCon)
{
	Camera* playerCam = mPlayer->GetCamera();

	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mSolid;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	Effects::TerrainFX->SetEyePos(playerCam->GetPosition());
	Effects::NormalFX->SetEyePos(playerCam->GetPosition());

	activeTech = Effects::TerrainFX->mAllLights;
	mTerrain->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::NormalFX->mAllLightsAlpha;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->Draw(devCon, activeTech, playerCam);
	}
}

// Tessellation draw methods

void Game::SolidTessDraw(ID3D11DeviceContext* devCon)
{
	this->SetTessEffects();
	Camera* playerCam = mPlayer->GetCamera();

	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mSolid;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::TerrTessFX->mSolid;
	mTerrain->DrawTess(devCon, activeTech, playerCam);

	activeTech = Effects::TessFX->mSolidAlpha;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->DrawTess(devCon, activeTech, playerCam);
	}
}

void Game::WireTessDraw(ID3D11DeviceContext* devCon)
{
	this->SetTessEffects();
	Camera* playerCam = mPlayer->GetCamera();

	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mWire;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::TerrTessFX->mWire;
	mTerrain->DrawTess(devCon, activeTech, playerCam);

	activeTech = Effects::TessFX->mWire;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->DrawTess(devCon, activeTech, playerCam);
	}
}

void Game::LightTessDraw(ID3D11DeviceContext* devCon)
{
	this->SetTessEffects();
	Camera* playerCam = mPlayer->GetCamera();

	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mSolid;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::TerrTessFX->mAllLights;
	mTerrain->DrawTess(devCon, activeTech, playerCam);

	activeTech = Effects::TessFX->mAllLightsAlpha;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->DrawTess(devCon, activeTech, playerCam);
	}
}

// Shadow map drawing
void Game::ShadowMapDraw(ID3D11DeviceContext* devCon)
{
	Camera* playerCam = mPlayer->GetCamera();

	ID3DX11EffectTechnique* activeTech = Effects::SkyFX->mSolid;
	mSkyBox->Draw(devCon, activeTech, playerCam);

	Effects::TerrainFX->SetEyePos(playerCam->GetPosition());
	Effects::NormalFX->SetEyePos(playerCam->GetPosition());

	activeTech = Effects::TerrainFX->mShadow;
	mTerrain->Draw(devCon, activeTech, playerCam);

	activeTech = Effects::NormalFX->mShadowAlpha;
	for(UINT i(0); i != mPlatforms.size(); ++i)
	{
		mPlatforms[i]->Draw(devCon, activeTech, playerCam);
	}
}

void Game::ControlPlayer(DirectInput* di)
{
	mPlayer->Control(di);
}

void Game::SetTessEffects()
{
	Effects::TessFX->SetMinTessDist(10.0f);
	Effects::TessFX->SetMaxTessDist(1000.0f);
	Effects::TessFX->SetMinTessFact(1.0f);
	Effects::TessFX->SetMaxTessFact(2.0f);
	Effects::TessFX->SetEyePos(mPlayer->GetCamera()->GetPosition());

	Effects::TerrTessFX->SetMinTessDist(100.0f);
	Effects::TerrTessFX->SetMaxTessDist(1000.0f);
	Effects::TerrTessFX->SetMinTessFact(1.0f);
	Effects::TerrTessFX->SetMaxTessFact(2.0f);
	Effects::TerrTessFX->SetEyePos(mPlayer->GetCamera()->GetPosition());
}