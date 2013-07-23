#include "LightHandler.h"

LightHandler::LightHandler(void)
	//: mDirAmount(1), mPointAmount(2), mSpotAmount(2)
	//mDirs(new DirectionalLight[mDirAmount]), 
	//mPoints(new PointLight[mPointAmount]),
	//mSpots(new SpotLight[mSpotAmount])
{
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
	mPoints[0].Range		= 900.0f;
	mPoints[0].Position = XMFLOAT3(1500.0f, 50.0f, 1500.0f);

	mPoints[1].Ambient		= XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mPoints[1].Diffuse		= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	mPoints[1].Specular		= XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mPoints[1].Attenuation  = XMFLOAT3(1.0f, 1.0f, 1.0f);
	mPoints[1].Range		= 400.0f;
	mPoints[1].Position		= XMFLOAT3(400.0f, 400.0f, 100.0f);

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
	//delete[] mDirs;
	//delete[] mPoints;
	//delete[] mSpots;
}

void LightHandler::Update(float dt)
{
	static float time = 0.0f;
	time += dt;
	float x = 125.0f*cosf(1.0f*time) + 750.0f;
	float y = 125.0f*sinf(1.0f*time) + 1000.0f;
	float z = 750.0f;

	mPoints[0].Position = XMFLOAT3(x, y, z);
}

void LightHandler::ApplyEffects()
{
	Effects::TerrainFX->SetDirLights(mDirs, mDirAmount);
	Effects::TerrainFX->SetPointLights(mPoints, mPointAmount);
	Effects::TerrainFX->SetSpotLights(mSpots, mSpotAmount);

	Effects::NormalFX->SetDirLights(mDirs, mDirAmount);
	Effects::NormalFX->SetPointLights(mPoints, mPointAmount);
	Effects::NormalFX->SetSpotLights(mSpots, mSpotAmount);

	Effects::TessFX->SetDirLights(mDirs, mDirAmount);
	Effects::TessFX->SetPointLights(mPoints, mPointAmount);
	Effects::TessFX->SetSpotLights(mSpots, mSpotAmount);

	Effects::TerrTessFX->SetDirLights(mDirs, mDirAmount);
	Effects::TerrTessFX->SetPointLights(mPoints, mPointAmount);
	Effects::TerrTessFX->SetSpotLights(mSpots, mSpotAmount);
}
