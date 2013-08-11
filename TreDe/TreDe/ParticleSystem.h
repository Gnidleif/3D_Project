#pragma once
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H
#include "d3dUtilities.h"
#include "Effects.h"
#include "Camera.h"
#include "VertexDef.h"
#include "InputLayouts.h"
#include "TextureClass.h"

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* devCon, ParticleEffect* effect, std::string texPath, UINT maxParticles);
	void Update(float dt, float gametime);
	void Draw(Camera* camera);
	void Reset();

public:
	void SetEmitPos(XMFLOAT3 emitPos) { this->mEmitPos = emitPos; }
	void SetEmitDir(XMFLOAT3 emitDir) { this->mEmitDir = emitDir; }

public:
	float GetAge() const { return this->mAge; }

private:
	void BuildVB(ID3D11Device* device);

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDevCon;

	UINT mMaxParticles;
	bool mFirstRun;

	float mGameTime;
	float mTimeStep;
	float mAge;

	XMFLOAT3 mEmitPos;
	XMFLOAT3 mEmitDir;

	ID3D11ShaderResourceView* mTexArraySRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	ID3D11Buffer* mInitVB;
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;

	ParticleEffect* mEffect;
};
#endif