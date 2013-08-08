#pragma once
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
#include "SkyBox.h"
#include "LightHandler.h"

class Game
{
public:
	Game();
	~Game();
	void Initialize(ID3D11Device* device);
	void Update(float dt);

	void SolidDraw(ID3D11DeviceContext* devCon);
	void WireDraw(ID3D11DeviceContext* devCon);
	void LightDraw(ID3D11DeviceContext* devCon);
	void SolidTessDraw(ID3D11DeviceContext* devCon);
	void WireTessDraw(ID3D11DeviceContext* devCon);
	void LightTessDraw(ID3D11DeviceContext* devCon);
	void ShadowMapDraw(ID3D11DeviceContext* devCon);

	void ControlPlayer(DirectInput* di);

public:
	Camera* GetPlayerCam() const { return this->mPlayer->GetCamera(); }
	TerrainEntity* GetTerrain() const { return this->mTerrain; }
	std::vector<StaticEntity*> GetPlatforms() const { return this->mPlatforms; }

private:
	void SetTessEffects();

private:
	Player* mPlayer;
	TerrainEntity* mTerrain;
	std::vector<StaticEntity*> mPlatforms;
	//SkinnedEntity* mCharacter;
	SkyBox* mSkyBox;
};
#endif