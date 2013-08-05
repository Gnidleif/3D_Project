#include "ShadowMap.h"
#include "Effects.h"

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
	CreateMap();
}

void ShadowMap::CreateMap()
{
	// Try changing the .Format stuff on the descriptions if shit fucks up
	mWidth = Settings->GetData()->mWidth;
	mHeight = Settings->GetData()->mHeight;
	SafeRelease(mDepthSRV);
	SafeRelease(mDepthDSV);
	Effects::ShadowFX->SetResX(mWidth);
	Effects::ShadowFX->SetResY(mHeight);

	this->mViewport.TopLeftX = 0;
	this->mViewport.TopLeftY = 0;
	this->mViewport.Width = (float)this->mWidth;
	this->mViewport.Height = (float)this->mHeight;
	this->mViewport.MinDepth = 0.0f;
	this->mViewport.MaxDepth = 1.0f;

	D3D11_TEXTURE2D_DESC texDesc = {};

	texDesc.Width = this->mWidth;
	texDesc.Height = this->mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* lpDepthMap = nullptr;
	if(FAILED(mDevice->CreateTexture2D(&texDesc, 0, &lpDepthMap)))
		MessageBox(0, "Error Creating DepthMap", "Error", 0);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	if(FAILED(mDevice->CreateDepthStencilView(lpDepthMap, &dsvDesc, &mDepthDSV)))
		MessageBox(0, "Error creating DepthMap DSV", "Error" , 0);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	if(FAILED(mDevice->CreateShaderResourceView(lpDepthMap, &srvDesc, &mDepthSRV)))
		MessageBox(0, "Error creating DepthMap SRV" , "Error" , 0);

	SafeRelease(lpDepthMap);
}