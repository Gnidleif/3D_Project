#ifndef GAME_H
#define GAME_H
#include "SettingsClass.h"
#include "DirectInput.h"
#include "TextClass.h"
#include "LoaderClass.h"
#include "Player.h"
#include "TerrainEntity.h"
#include "StaticEntity.h"
#include "SkinnedEntity.h"

class Game
{
public:
	Game();
	~Game();
	void Initialize();
	void Update(float dt);
	void SolidDraw(ID3D11DeviceContext* devCon);
	void WireDraw(ID3D11DeviceContext* devCon);
	void ControlPlayer(DirectInput* di);

public:
	Camera* GetPlayerCam() { return this->mPlayer->GetCamera(); }

private:
	Player* mPlayer;
	TerrainEntity* mTerrain;
	StaticEntity* mPlatform;
	SkinnedEntity* mCharacter;
};
#endif