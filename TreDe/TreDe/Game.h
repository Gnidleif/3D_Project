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

class Game
{
public:
	Game();
	~Game();
	void Initialize(ID3D11Device* device);
	void Update(float dt);
	void SolidDraw(ID3D11DeviceContext* devCon, bool lightsOn);
	void WireDraw(ID3D11DeviceContext* devCon);
	void ControlPlayer(DirectInput* di);

public:
	Camera* GetPlayerCam() { return this->mPlayer->GetCamera(); }

private:
	Player* mPlayer;
	TerrainEntity* mTerrain;
	std::vector<StaticEntity*> mPlatforms;
	//SkinnedEntity* mCharacter;
	SkyBox* mSkyBox;
	std::vector<StaticEntity*> mDucks;

	UINT mDirLightAmount;
	UINT mPointLightAmount;
	UINT mSpotLightAmount;
	DirectionalLight* mDirLights;
	PointLight* mPointLights;
	SpotLight* mSpotLights;
};
#endif