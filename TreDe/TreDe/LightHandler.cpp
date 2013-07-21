#include "LightHandler.h"

LightHandler::LightHandler(void)
	: mDirAmount(1), mPointAmount(2), mSpotAmount(2),
	mDirs(new DirectionalLight[mDirAmount]), 
	mPoints(new PointLight[mPointAmount]),
	mSpots(new SpotLight[mSpotAmount])
{
	// Directional lights initialized

	mDirs[0].Ambient = LightLights::WHITE;
	mDirs[0].Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	mDirs[0].Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirs[0].Direction = XMFLOAT3(-0.5f, -0.5f, 0.5f);

	// Pointlights initialized

	mPoints[0].Ambient = LightLights::WHITE;
	mPoints[0].Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	mPoints[0].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mPoints[0].Attenuation = XMFLOAT3(1.0f, 1.0f, 1.0f);
	mPoints[0].Range = 100.0f;
	mPoints[0].Position = XMFLOAT3(200.0f, 15.0f, 200.0f);

	mPoints[0].Ambient = LightLights::WHITE;
	mPoints[0].Diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	mPoints[0].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mPoints[0].Attenuation = XMFLOAT3(1.0f, 1.0f, 1.0f);
	mPoints[0].Range = 100.0f;
	mPoints[0].Position = XMFLOAT3(0.0f, 15.0f, 0.0f);

	// Spotlights initialized

	mSpots[0].Ambient = LightLights::WHITE;
	mSpots[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpots[0].Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpots[0].Position = XMFLOAT3(0.0f, 25.0f, 200.0f);
	mSpots[0].Range = 75.0f;
	mSpots[0].Direction = XMFLOAT3(0.5f, 0.5f, 0.5f);

	mSpots[1].Ambient = LightLights::WHITE;
	mSpots[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpots[1].Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpots[1].Position = XMFLOAT3(200.0f, 25.0f, 200.0f);
	mSpots[1].Range = 75.0f;
	mSpots[1].Direction = XMFLOAT3(0.5f, 0.5f, 0.5f);
}


LightHandler::~LightHandler(void)
{
	delete[] mDirs;
	delete[] mPoints;
	delete[] mSpots;
}

void LightHandler::Update(float dt)
{
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
