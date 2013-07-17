#ifndef EFFECTS_H
#define EFFECTS_H
#include "d3dUtilities.h"

// Base effect class
class VirtualEffect
{
public:
	VirtualEffect(ID3D11Device* device, std::string filename);
	virtual ~VirtualEffect();

protected:
	ID3DX11Effect* mFX;
};

// Effect class for the skybox
class SkyEffect : public VirtualEffect
{
public:
	SkyEffect(ID3D11Device* device, std::string filename);
	~SkyEffect() {}

public:
	void SetWVP(XMMATRIX* matrix) { this->mWVP->SetMatrix(reinterpret_cast<const float*>(matrix)); }
	void SetCubeMap(ID3D11ShaderResourceView* cubeMap) { this->mCubeMap->SetResource(cubeMap); }

public: // Techniques
	ID3DX11EffectTechnique* mSkyTech;
	ID3DX11EffectTechnique* mWireSkyTech;

private:
	ID3DX11EffectMatrixVariable* mWVP;
	ID3DX11EffectShaderResourceVariable* mCubeMap;
};

// Effect class for the terrain
class TerrainEffect : public VirtualEffect
{
public:
	TerrainEffect(ID3D11Device* device, std::string filename);
	~TerrainEffect() {}

public:
	void SetWorld(XMMATRIX* matrix) { this->mWorld->SetMatrix(reinterpret_cast<const float*>(matrix)); }
	void SetView(XMMATRIX* matrix) { this->mView->SetMatrix(reinterpret_cast<const float*>(matrix)); }
	void SetProj(XMMATRIX* matrix) { this->mProj->SetMatrix(reinterpret_cast<const float*>(matrix)); }
	void SetWorldInvTranspose(XMMATRIX* matrix) { this->mWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(matrix)); }

	// Set textures
	void SetBlendMap(ID3D11ShaderResourceView* bm) { this->mBlendMap->SetResource(bm); }
	void SetTex0(ID3D11ShaderResourceView* t0) { this->mTex0->SetResource(t0); }
	void SetTex1(ID3D11ShaderResourceView* t1) { this->mTex1->SetResource(t1); }
	void SetTex2(ID3D11ShaderResourceView* t2) { this->mTex2->SetResource(t2); }
	void SetTex3(ID3D11ShaderResourceView* t3) { this->mTex3->SetResource(t3); }
	// Set material
	void SetMaterial(Material* mat) { mMaterial->SetRawValue(&mat, 0, sizeof(Material)); }

public:
	ID3DX11EffectTechnique* mTerrainSolidTech;
	ID3DX11EffectTechnique* mTerrainWireTech;

private:
	ID3DX11EffectMatrixVariable* mWorld;
	ID3DX11EffectMatrixVariable* mView;
	ID3DX11EffectMatrixVariable* mProj;
	ID3DX11EffectMatrixVariable* mWorldInvTranspose;

	ID3DX11EffectShaderResourceVariable* mBlendMap;
	ID3DX11EffectShaderResourceVariable* mTex0;
	ID3DX11EffectShaderResourceVariable* mTex1;
	ID3DX11EffectShaderResourceVariable* mTex2;
	ID3DX11EffectShaderResourceVariable* mTex3;

	ID3DX11EffectVariable* mMaterial;
};

// Normal map effect class
class NormalEffect : public VirtualEffect
{
public:
	NormalEffect(ID3D11Device* device, std::string filename);
	~NormalEffect() {}

public:
	void SetWorld(XMMATRIX* matrix) { this->mWorld->SetMatrix(reinterpret_cast<const float*>(matrix)); }
	void SetView(XMMATRIX* matrix) { this->mView->SetMatrix(reinterpret_cast<const float*>(matrix)); }
	void SetProj(XMMATRIX* matrix) { this->mProj->SetMatrix(reinterpret_cast<const float*>(matrix)); }
	void SetWorldInvTranspose(XMMATRIX* matrix) { this->mWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(matrix)); }

	void SetBoneTransforms(XMFLOAT4X4* matrix, int count) { mBoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(matrix), 0, count); }

	void SetDiffuseMap(ID3D11ShaderResourceView* diffMap) { this->mDiffuseMap->SetResource(diffMap); }
	void SetNormalMap(ID3D11ShaderResourceView* normMap) { this->mNormalMap->SetResource(normMap); }

	void SetMaterial(Material* mat) { mMaterial->SetRawValue(&mat, 0, sizeof(Material)); }

public: // Techniques
	ID3DX11EffectTechnique* mNormalSolidTech;
	ID3DX11EffectTechnique* mNormalSolidAlphaTech;
	ID3DX11EffectTechnique* mNormalWireTech;

	ID3DX11EffectTechnique* mNormalMapSolidAlphaSkinTech;

private:
	ID3DX11EffectMatrixVariable* mWorld;
	ID3DX11EffectMatrixVariable* mView;
	ID3DX11EffectMatrixVariable* mProj;
	ID3DX11EffectMatrixVariable* mWorldInvTranspose;

	ID3DX11EffectMatrixVariable* mBoneTransforms;

	ID3DX11EffectShaderResourceVariable* mDiffuseMap;
	ID3DX11EffectShaderResourceVariable* mNormalMap;

	ID3DX11EffectVariable* mMaterial;
};

// Class gathering static objects of all the other effectclasses
class Effects
{
public:
	static void Initialize(ID3D11Device* device);
	static void Shutdown();

	static SkyEffect* SkyFX;
	static TerrainEffect* TerrainFX;
	static NormalEffect* NormalFX;
};
#endif