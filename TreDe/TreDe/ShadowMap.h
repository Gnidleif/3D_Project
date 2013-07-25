#pragma once
#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include "d3dUtilities.h"
#include "StaticEntity.h"
#include "Camera.h"
#include "SettingsClass.h"
#include "LightDef.h"

class ShadowMap
{
private:
	ID3D11Device* mDevice;
	UINT mWidth;
	UINT mHeight;

	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;

	ID3D11ShaderResourceView* mDepthMapSRV;
	ID3D11DepthStencilView* mDepthMapDSV;

	D3D11_VIEWPORT mViewport;

	void CreateShadowmap();

public:
	ShadowMap();
	~ShadowMap(void);
	void Initialize(ID3D11Device* device);
	void BindDSVAndSetRenderTarget(ID3D11DeviceContext* devCon);
	void UpdateResolution();

public:
	ID3D11ShaderResourceView* GetDepthMapSRV() const { return this->mDepthMapSRV; }
};
#endif