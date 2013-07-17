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
	SafeDelete(mCharacter);
}

void Game::Initialize()
{
	mTerrain = new TerrainEntity("../Data/Textures/heightmap.bmp");
	mTerrain->Initialize(XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f);

	mPlatform = new StaticEntity("../Data/Models/Static/Platform1/Platform1.obj", "../Data/Models/Static/Platform1/");
	mPlatform->Initialize(XMFLOAT3(100.0f, 20.0f, 100.0f), 0.05f);

	mCharacter = new SkinnedEntity("../Data/Models/Skinned/Character/Character.dae", "../Data/Models/Skinned/Character/");
	mCharacter->Initialize(XMFLOAT3(150.0f, 20.0f, 100.0f), 1.0f);
	//
	Text->AddConstantText("PlayerInfo", "Name: " + mPlayer->GetName(), 20.0f, 20.0f, 20.0f, TextColors::White);
}

void Game::Update(float dt)
{
	Text->AddConstantText("CamRot", "Rotation: " + Text->XMFLOAT3ToString(mPlayer->GetCamera()->GetYawPitchRoll()), 20.0f, 50.0f, 20.0f, TextColors::Red);
	Text->AddConstantText("CamPos", "Position: " + Text->XMFLOAT3ToString(mPlayer->GetPosition()), 20.0f, 80.0f, 20.0f, TextColors::Red);
	//static float rotation = 0.0f;
	//rotation += dt;
	//mTerrain->RotateXYZ(XMFLOAT3(rotation, rotation, rotation));

	mCharacter->Update(dt);
	mPlayer->Update(dt);
}

void Game::SolidDraw(ID3D11DeviceContext* devCon)
{
	ID3DX11EffectTechnique* activeTech = Effects::TerrainFX->mTerrainSolidTech;
	mTerrain->Draw(devCon, activeTech, mPlayer->GetCamera());

	activeTech = Effects::NormalFX->mNormalSolidAlphaTech;
	mPlatform->Draw(devCon, activeTech, mPlayer->GetCamera());
}

void Game::WireDraw(ID3D11DeviceContext* devCon)
{
	ID3DX11EffectTechnique* activeTech = Effects::TerrainFX->mTerrainWireTech;
	mTerrain->Draw(devCon, activeTech, mPlayer->GetCamera());

	activeTech = Effects::NormalFX->mNormalWireTech;
	mPlatform->Draw(devCon, activeTech, mPlayer->GetCamera());
}

void Game::ControlPlayer(DirectInput* di)
{
	if(di->GetKeyboardState()[DIK_R] && 0x80)
	{
		mPlatform->SetScale(0.05f);
	}
	else if(di->GetKeyboardState()[DIK_F] && 0x80)
	{
		mPlatform->SetScale(0.1f);
	}
	else
		mPlayer->Control(di);
}