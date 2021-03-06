#pragma once
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
	ID3DX11EffectTechnique* mSolid;
	ID3DX11EffectTechnique* mWire;

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
	// Set eye pos, try changing this later if something doesn't work to see what happens
	void SetEyePos(const XMFLOAT3& eye) { this->mEyePos->SetRawValue(&eye, 0, sizeof(XMFLOAT3)); }

	void SetWorld(const XMMATRIX& matrix) { this->mWorld->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetView(const XMMATRIX& matrix) { this->mView->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetProj(const XMMATRIX& matrix) { this->mProj->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetWorldInvTranspose(const XMMATRIX& matrix) { this->mWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetLightVP(const XMMATRIX& matrix) { this->mLightVP->SetMatrix(reinterpret_cast<const float*>(&matrix)); }

	// Set textures
	void SetBlendMap(ID3D11ShaderResourceView* bm) { this->mBlendMap->SetResource(bm); }
	void SetTex0(ID3D11ShaderResourceView* t0) { this->mTex0->SetResource(t0); }
	void SetTex1(ID3D11ShaderResourceView* t1) { this->mTex1->SetResource(t1); }
	void SetTex2(ID3D11ShaderResourceView* t2) { this->mTex2->SetResource(t2); }
	void SetTex3(ID3D11ShaderResourceView* t3) { this->mTex3->SetResource(t3); }
	void SetShadowMap(ID3D11ShaderResourceView* shadMap) { this->mShadowMap->SetResource(shadMap); }

	// Set lights
	void SetDirLights(const DirectionalLight* lights, int amount) { this->mDirLights->SetRawValue(lights, 0, amount*sizeof(DirectionalLight)); }
	void SetPointLights(const PointLight* lights, int amount) { this->mPointLights->SetRawValue(lights, 0, amount*sizeof(PointLight)); }
	void SetSpotLights(const SpotLight* lights, int amount) { this->mSpotLights->SetRawValue(lights, 0, amount*sizeof(SpotLight)); }
	// Set material
	void SetMaterial(const Material& mat) { mMaterial->SetRawValue(&mat, 0, sizeof(Material)); }

public:
	ID3DX11EffectTechnique* mSolid;
	ID3DX11EffectTechnique* mWire;
	ID3DX11EffectTechnique* mAllLights;
	ID3DX11EffectTechnique* mShadow;

private:
	ID3DX11EffectVectorVariable* mEyePos;

	ID3DX11EffectMatrixVariable* mWorld;
	ID3DX11EffectMatrixVariable* mView;
	ID3DX11EffectMatrixVariable* mProj;
	ID3DX11EffectMatrixVariable* mWorldInvTranspose;
	ID3DX11EffectMatrixVariable* mLightVP;

	ID3DX11EffectShaderResourceVariable* mBlendMap;
	ID3DX11EffectShaderResourceVariable* mTex0;
	ID3DX11EffectShaderResourceVariable* mTex1;
	ID3DX11EffectShaderResourceVariable* mTex2;
	ID3DX11EffectShaderResourceVariable* mTex3;
	ID3DX11EffectShaderResourceVariable* mShadowMap;

	ID3DX11EffectVariable* mDirLights;
	ID3DX11EffectVariable* mPointLights;
	ID3DX11EffectVariable* mSpotLights;
	ID3DX11EffectVariable* mMaterial;
};

// Normal map effect class
class NormalEffect : public VirtualEffect
{
public:
	NormalEffect(ID3D11Device* device, std::string filename);
	~NormalEffect() {}

public:
	void SetEyePos(const XMFLOAT3& eye) { this->mEyePos->SetRawValue(&eye, 0, sizeof(XMFLOAT3)); }

	void SetWorld(const XMMATRIX& matrix) { this->mWorld->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetView(const XMMATRIX& matrix) { this->mView->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetProj(const XMMATRIX& matrix) { this->mProj->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetWorldInvTranspose(const XMMATRIX& matrix) { this->mWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetLightVP(const XMMATRIX& matrix) { this->mLightVP->SetMatrix(reinterpret_cast<const float*>(&matrix)); }

	void SetBoneTransforms(XMFLOAT4X4* matrix, int count) { mBoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(matrix), 0, count); }

	void SetDiffuseMap(ID3D11ShaderResourceView* diffMap) { this->mDiffuseMap->SetResource(diffMap); }
	void SetNormalMap(ID3D11ShaderResourceView* normMap) { this->mNormalMap->SetResource(normMap); }
	void SetShadowMap(ID3D11ShaderResourceView* shadMap) { this->mShadowMap->SetResource(shadMap); }

	// Set lights
	void SetDirLights(const DirectionalLight* lights, int amount) { this->mDirLights->SetRawValue(lights, 0, amount*sizeof(DirectionalLight)); }
	void SetPointLights(const PointLight* lights, int amount) { this->mPointLights->SetRawValue(lights, 0, amount*sizeof(PointLight)); }
	void SetSpotLights(const SpotLight* lights, int amount) { this->mSpotLights->SetRawValue(lights, 0, amount*sizeof(SpotLight)); }
	void SetMaterial(const Material& mat) { mMaterial->SetRawValue(&mat, 0, sizeof(Material)); }

public: // Techniques
	ID3DX11EffectTechnique* mSolid;
	ID3DX11EffectTechnique* mSolidSkin;
	ID3DX11EffectTechnique* mSolidAlpha;
	ID3DX11EffectTechnique* mSolidAlphaSkin;
	ID3DX11EffectTechnique* mWire;
	ID3DX11EffectTechnique* mAllLights;
	ID3DX11EffectTechnique* mAllLightsAlpha;
	ID3DX11EffectTechnique* mShadowAlpha;

private:
	ID3DX11EffectVectorVariable* mEyePos;

	ID3DX11EffectMatrixVariable* mWorld;
	ID3DX11EffectMatrixVariable* mView;
	ID3DX11EffectMatrixVariable* mProj;
	ID3DX11EffectMatrixVariable* mWorldInvTranspose;
	ID3DX11EffectMatrixVariable* mLightVP;

	ID3DX11EffectMatrixVariable* mBoneTransforms;

	ID3DX11EffectShaderResourceVariable* mDiffuseMap;
	ID3DX11EffectShaderResourceVariable* mNormalMap;
	ID3DX11EffectShaderResourceVariable* mShadowMap;

	ID3DX11EffectVariable* mDirLights;
	ID3DX11EffectVariable* mPointLights;
	ID3DX11EffectVariable* mSpotLights;
	ID3DX11EffectVariable* mMaterial;
};

// Effect-class for tessellation, might right now it will (maybe) work for common NormalMap-objects, but I don't know
class TessellationEffect : public VirtualEffect
{
public:
	TessellationEffect(ID3D11Device* device, std::string filename);
	~TessellationEffect() {}

public:
	void SetEyePos(const XMFLOAT3& eye) { this->mEyePos->SetRawValue(&eye, 0, sizeof(XMFLOAT3)); }

	void SetWorld(const XMMATRIX& matrix) { this->mWorld->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetView(const XMMATRIX& matrix) { this->mView->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetProj(const XMMATRIX& matrix) { this->mProj->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetWorldInvTranspose(const XMMATRIX& matrix) { this->mWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&matrix)); }

	void SetDiffuseMap(ID3D11ShaderResourceView* diffMap) { this->mDiffuseMap->SetResource(diffMap); }
	void SetNormalMap(ID3D11ShaderResourceView* normMap) { this->mNormalMap->SetResource(normMap); }

	// Set lights
	void SetDirLights(const DirectionalLight* lights, int amount) { this->mDirLights->SetRawValue(lights, 0, amount*sizeof(DirectionalLight)); }
	void SetPointLights(const PointLight* lights, int amount) { this->mPointLights->SetRawValue(lights, 0, amount*sizeof(PointLight)); }
	void SetSpotLights(const SpotLight* lights, int amount) { this->mSpotLights->SetRawValue(lights, 0, amount*sizeof(SpotLight)); }
	void SetMaterial(const Material& mat) { mMaterial->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetMinTessDist(float data) { this->mMinDist->SetFloat(data); }
	void SetMaxTessDist(float data) { this->mMaxDist->SetFloat(data); }
	void SetMinTessFact(float data) { this->mMinTess->SetFloat(data); }
	void SetMaxTessFact(float data) { this->mMaxTess->SetFloat(data); }

public: // Techniques
	ID3DX11EffectTechnique* mSolid;
	ID3DX11EffectTechnique* mSolidAlpha;
	ID3DX11EffectTechnique* mWire;
	ID3DX11EffectTechnique* mAllLights;
	ID3DX11EffectTechnique* mAllLightsAlpha;

private:
	ID3DX11EffectVectorVariable* mEyePos;

	ID3DX11EffectMatrixVariable* mWorld;
	ID3DX11EffectMatrixVariable* mView;
	ID3DX11EffectMatrixVariable* mProj;
	ID3DX11EffectMatrixVariable* mWorldInvTranspose;

	ID3DX11EffectShaderResourceVariable* mDiffuseMap;
	ID3DX11EffectShaderResourceVariable* mNormalMap;
	
	ID3DX11EffectVariable* mDirLights;
	ID3DX11EffectVariable* mPointLights;
	ID3DX11EffectVariable* mSpotLights;
	ID3DX11EffectVariable* mMaterial;

	ID3DX11EffectScalarVariable* mMinDist;
	ID3DX11EffectScalarVariable* mMaxDist;
	ID3DX11EffectScalarVariable* mMinTess;
	ID3DX11EffectScalarVariable* mMaxTess;
};

class TerrainTessellationEffect : public VirtualEffect
{
public:
	TerrainTessellationEffect(ID3D11Device* device, std::string filename);
	~TerrainTessellationEffect() {}

public:
	void SetEyePos(const XMFLOAT3& eye) { this->mEyePos->SetRawValue(&eye, 0, sizeof(XMFLOAT3)); }

	void SetWorld(const XMMATRIX& matrix) { this->mWorld->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetView(const XMMATRIX& matrix) { this->mView->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetProj(const XMMATRIX& matrix) { this->mProj->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetWorldInvTranspose(const XMMATRIX& matrix) { this->mWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&matrix)); }

	void SetBlendMap(ID3D11ShaderResourceView* bm) { this->mBlendMap->SetResource(bm); }
	void SetTex0(ID3D11ShaderResourceView* t0) { this->mTex0->SetResource(t0); }
	void SetTex1(ID3D11ShaderResourceView* t1) { this->mTex1->SetResource(t1); }
	void SetTex2(ID3D11ShaderResourceView* t2) { this->mTex2->SetResource(t2); }
	void SetTex3(ID3D11ShaderResourceView* t3) { this->mTex3->SetResource(t3); }

	// Set lights
	void SetDirLights(const DirectionalLight* lights, int amount) { this->mDirLights->SetRawValue(lights, 0, amount*sizeof(DirectionalLight)); }
	void SetPointLights(const PointLight* lights, int amount) { this->mPointLights->SetRawValue(lights, 0, amount*sizeof(PointLight)); }
	void SetSpotLights(const SpotLight* lights, int amount) { this->mSpotLights->SetRawValue(lights, 0, amount*sizeof(SpotLight)); }
	void SetMaterial(const Material& mat) { mMaterial->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetMinTessDist(float data) { this->mMinDist->SetFloat(data); }
	void SetMaxTessDist(float data) { this->mMaxDist->SetFloat(data); }
	void SetMinTessFact(float data) { this->mMinTess->SetFloat(data); }
	void SetMaxTessFact(float data) { this->mMaxTess->SetFloat(data); }

public: // Techniques
	ID3DX11EffectTechnique* mSolid;
	ID3DX11EffectTechnique* mWire;
	ID3DX11EffectTechnique* mAllLights;

private:
	ID3DX11EffectVectorVariable* mEyePos;

	ID3DX11EffectMatrixVariable* mWorld;
	ID3DX11EffectMatrixVariable* mView;
	ID3DX11EffectMatrixVariable* mProj;
	ID3DX11EffectMatrixVariable* mWorldInvTranspose;

	ID3DX11EffectShaderResourceVariable* mBlendMap;
	ID3DX11EffectShaderResourceVariable* mTex0;
	ID3DX11EffectShaderResourceVariable* mTex1;
	ID3DX11EffectShaderResourceVariable* mTex2;
	ID3DX11EffectShaderResourceVariable* mTex3;

	ID3DX11EffectVariable* mDirLights;
	ID3DX11EffectVariable* mPointLights;
	ID3DX11EffectVariable* mSpotLights;
	ID3DX11EffectVariable* mMaterial;

	ID3DX11EffectScalarVariable* mMinDist;
	ID3DX11EffectScalarVariable* mMaxDist;
	ID3DX11EffectScalarVariable* mMinTess;
	ID3DX11EffectScalarVariable* mMaxTess;
};

class ShadowMapEffect : public VirtualEffect
{
public:
	ShadowMapEffect(ID3D11Device* device, std::string filename);
	~ShadowMapEffect() {}

public:
	void SetWorld(const XMMATRIX& matrix) { this->mWorld->SetMatrix(reinterpret_cast<const float*>(&matrix)); }
	void SetLightVP(const XMMATRIX& matrix) { this->mLightVP->SetMatrix(reinterpret_cast<const float*>(&matrix)); }

	void SetScreenX(float data) { this->mScreenX->SetFloat(data); }
	void SetScreenY(float data) { this->mScreenY->SetFloat(data); }

public:
	ID3DX11EffectTechnique* mShadowTech;

private:
	ID3DX11EffectMatrixVariable* mWorld;
	ID3DX11EffectMatrixVariable* mLightVP;

	ID3DX11EffectScalarVariable* mScreenX;
	ID3DX11EffectScalarVariable* mScreenY;
};

class ParticleEffect : public VirtualEffect
{
public:
	ParticleEffect(ID3D11Device* device, std::string filename);
	~ParticleEffect() {}
	// Yeah that's right, only one technique per particleeffect!
	virtual ID3DX11EffectTechnique* GetTechnique() const = 0;

public:
	void SetViewProj(const XMMATRIX& matrix) { this->mViewProj->SetMatrix(reinterpret_cast<const float*>(&matrix)); }

	void SetEyePos(const XMFLOAT3& vector) { this->mEyePos->SetRawValue(&vector, 0, sizeof(XMFLOAT3)); }
	void SetEmitPos(const XMFLOAT3& vector) { this->mEmitPos->SetRawValue(&vector, 0, sizeof(XMFLOAT3)); }
	void SetEmitDir(const XMFLOAT3& vector) { this->mEmitDir->SetRawValue(&vector, 0, sizeof(XMFLOAT3)); }

	void SetGameTime(float data) { this->mGameTime->SetFloat(data); }
	void SetTimeStep(float data) { this->mTimeStep->SetFloat(data); }

	void SetTexArray(ID3D11ShaderResourceView* tex) { this->mTexArray->SetResource(tex); }
	void SetRandomTex(ID3D11ShaderResourceView* tex) { this->mRandomTex->SetResource(tex); }

protected:
	ID3DX11EffectMatrixVariable* mViewProj;

	ID3DX11EffectVectorVariable* mEyePos;
	ID3DX11EffectVectorVariable* mEmitPos;
	ID3DX11EffectVectorVariable* mEmitDir;

	ID3DX11EffectScalarVariable* mGameTime;
	ID3DX11EffectScalarVariable* mTimeStep;

	ID3DX11EffectShaderResourceVariable* mTexArray;
	ID3DX11EffectShaderResourceVariable* mRandomTex;
};

class SunEffect : public ParticleEffect
{
public:
	SunEffect(ID3D11Device* device, std::string filename);
	~SunEffect() {}
	ID3DX11EffectTechnique* GetTechnique() const { return this->mDraw; }

private:
	ID3DX11EffectTechnique* mDraw;
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
	static TessellationEffect* TessFX;
	static TerrainTessellationEffect* TerrTessFX;
	static ShadowMapEffect* ShadowFX;
	static SunEffect* SunFX;
};
#endif