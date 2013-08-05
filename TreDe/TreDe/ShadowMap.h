#pragma once
#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include "d3dUtilities.h"
#include "SettingsClass.h"

class ShadowMap
{
public:
	ShadowMap(void);
	~ShadowMap(void);
	void Initialize(ID3D11Device* device);
	void CreateMap();

public:
	ID3D11ShaderResourceView* GetDepthSRV() const { return this->mDepthSRV; }

private:

private:
	ID3D11Device* mDevice;
	UINT mWidth;
	UINT mHeight;
	D3D11_VIEWPORT mViewport;
	ID3D11ShaderResourceView* mDepthSRV;
	ID3D11DepthStencilView* mDepthDSV;
};
#endif