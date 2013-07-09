#include "Game.h"
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
	SafeDelete(mPlatform);
}

void Game::Initialize()
{
	mTerrain = new TerrainEntity("../Data/Textures/heightmap.bmp");
	mTerrain->Initialize(XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f);

	mPlatform = new StaticEntity("../Data/Models/Static/Platform1/Platform1.obj", "../Data/Models/Static/Platform1/");
	mPlatform->Initialize(XMFLOAT3(50.0f, 50.0f, 50.0f), 0.01f);

	Text->AddConstantText("PlayerInfo", "Name: " + mPlayer->GetName(), 20.0f, 20.0f, 20.0f, TextColors::White);
}

void Game::Update(float dt)
{
	Text->AddConstantText("CamRot", "Rotation: " + Text->XMFLOAT3ToString(mPlayer->GetCamera()->GetYawPitchRoll()), 20.0f, 50.0f, 20.0f, TextColors::Red);
	Text->AddConstantText("CamPos", "Position: " + Text->XMFLOAT3ToString(mPlayer->GetPosition()), 20.0f, 80.0f, 20.0f, TextColors::Red);
	//static float rotation = 0.0f;
	//rotation += dt;
	//mTerrain->RotateXYZ(XMFLOAT3(rotation, rotation, rotation));

	mPlayer->Update(dt);
}

void Game::Draw(ID3D11DeviceContext* devCon)
{
	ID3DX11EffectTechnique* activeTech = Effects::TerrainFX->mTerrainSolidTech;
	mTerrain->Draw(devCon, activeTech, mPlayer->GetCamera());

	activeTech = Effects::NormalFX->mNormalSolidTech;
	mPlatform->Draw(devCon, activeTech, mPlayer->GetCamera());
}

void Game::ControlPlayer(DirectInput* di)
{
	mPlayer->Control(di);
}