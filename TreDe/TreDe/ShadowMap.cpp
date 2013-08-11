#include "ShadowMap.h"
#include "Effects.h"
#include <iostream>
using namespace std;

ShadowMap::ShadowMap()
	: 
	mDevice(nullptr),
	mWidth(0),
	mHeight(0),
	mDepthSRV(nullptr),
	mDepthDSV(nullptr)
{
	ZeroMemory(&mViewport, sizeof(D3D11_VIEWPORT));
}

ShadowMap::~ShadowMap(void)
{
	SafeRelease(mDepthSRV);
	SafeRelease(mDepthDSV);
}

void ShadowMap::Initialize(ID3D11Device* device)
{
	this->mDevice = device;
	ResetMap();
}

void ShadowMap::BindDSVAndSetRTV(ID3D11DeviceContext* devCon)
{
	devCon->RSSetViewports(1, &mViewport);

	ID3D11RenderTargetView* renderTargets[1] = {0};
	devCon ->OMSetRenderTargets(1, renderTargets, mDepthDSV);
	devCon->ClearDepthStencilView(mDepthDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowMap::ResetMap()
{
	UINT width = Settings->GetData()->mWidth;
	UINT height = Settings->GetData()->mHeight;

	if(mWidth != width && mHeight != height)
	{
		SafeRelease(mDepthSRV);
		SafeRelease(mDepthDSV);
		mWidth = width;
		mHeight = height;
		Effects::ShadowFX->SetScreenX(static_cast<float>(mWidth));
		Effects::ShadowFX->SetScreenY(static_cast<float>(mHeight));
		CreateMap();
		
		cout << "Shadow map size: " << mWidth << "x" << mHeight << "..." << endl;
	}
}

void ShadowMap::CreateMap()
{
	// Try changing the .Format stuff on the descriptions if shit fucks up

	this->mViewport.TopLeftX = 0;
	this->mViewport.TopLeftY = 0;
	this->mViewport.Width = static_cast<float>(this->mWidth);
	this->mViewport.Height = static_cast<float>(this->mHeight);
	this->mViewport.MinDepth = 0.0f;
	this->mViewport.MaxDepth = 1.0f;

	D3D11_TEXTURE2D_DESC texDesc = {};

	texDesc.Width = this->mWidth;
	texDesc.Height = this->mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* lpDepthMap = nullptr;
	if(FAILED(mDevice->CreateTexture2D(&texDesc, 0, &lpDepthMap)))
		MessageBox(0, "Error Creating DepthMap", "Error", 0);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	if(FAILED(mDevice->CreateDepthStencilView(lpDepthMap, &dsvDesc, &mDepthDSV)))
		MessageBox(0, "Error creating DepthMap DSV", "Error" , 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	if(FAILED(mDevice->CreateShaderResourceView(lpDepthMap, &srvDesc, &mDepthSRV)))
		MessageBox(0, "Error creating DepthMap SRV" , "Error" , 0);

	SafeRelease(lpDepthMap);
}