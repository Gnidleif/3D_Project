#include "Direct3D.h"
#include <iostream>
using namespace std;
#include "SettingsClass.h"

Direct3D::Direct3D(void)
	:
	mDevice(nullptr),
	mDevCon(nullptr),
	mSwapChain(nullptr),
	mDSBuffer(nullptr),
	mRTView(nullptr),
	mDSView(nullptr),

	mDriverType(D3D_DRIVER_TYPE_HARDWARE),
	mEnable4xMsaa(false),
	m4xMSAAQuality(0),
	mcWidth(nullptr),
	mcHeight(nullptr)
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
}

bool Direct3D::Initialize(HWND* hWnd)
{
	mcWidth = &Settings->GetData()->mWidth;
	mcHeight = &Settings->GetData()->mHeight;

	UINT createDeviceFlags = 0;
#if defined(_DEBUG) | (DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	if( FAILED ( D3D11CreateDevice(
		nullptr,
		mDriverType,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&mDevice,
		&featureLevel,
		&mDevCon)))
	{
		cout << "D3D11CreateDevice failed." << endl;
		return false;
	}

	if(featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		cout << "Direct3D feature level 11 unsupported." << endl;
		return false;
	}

	mDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMSAAQuality);
	assert(m4xMSAAQuality > 0);

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = *mcWidth;
	sd.BufferDesc.Height = *mcHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMSAAQuality-1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Use back buffer as render target
	sd.BufferCount = 1;								  // Number of back buffers to use in swap chain
	sd.OutputWindow = *hWnd;					  // Specify window we render into
	sd.Windowed = !Settings->GetData()->mIsFullscreen;	// Windowed mode or full-screen mode
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	      // Let display driver select most efficient presentation method
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGIDevice* dxgiDevice = nullptr;
	mDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter = nullptr;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter),(void**)&dxgiAdapter);

	IDXGIFactory* dxgiFactory = nullptr;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	dxgiFactory->CreateSwapChain(mDevice, &sd, &mSwapChain);

	SafeRelease(dxgiDevice);
	SafeRelease(dxgiAdapter);
	SafeRelease(dxgiFactory);

	this->OnResize();
	return true;
}

void Direct3D::OnResize()
{
	assert(mDevCon);
	assert(mDevice);
	assert(mSwapChain);

	SafeRelease(mRTView);
	SafeRelease(mDSView);
	SafeRelease(mDSBuffer);

	mSwapChain->ResizeBuffers(1, *mcWidth, *mcHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	ID3D11Texture2D* backBuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	mDevice->CreateRenderTargetView(backBuffer, 0, &mRTView);
	SafeRelease(backBuffer);

	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = *mcWidth;						// Texture width in texels
	depthStencilDesc.Height = *mcHeight;					// Texture height in texels
	depthStencilDesc.MipLevels = 1;								// Number of mipmap levels
	depthStencilDesc.ArraySize = 1;								// Number of textures in texture array
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// Texel format
	
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMSAAQuality-1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;			// How the texture will be used
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// Where the resource will be bound to the pipeline
	depthStencilDesc.CPUAccessFlags = 0;					// Specify CPU access (Only GPU writes/reads to the depth/buffer)
	depthStencilDesc.MiscFlags = 0;							// Optional flags

	mDevice->CreateTexture2D(&depthStencilDesc, 0, &mDSBuffer);
	mDevice->CreateDepthStencilView(mDSBuffer, 0, &mDSView);

	// Bind render target and depth/stencil view to the pipeline
	mDevCon->OMSetRenderTargets(1, &mRTView, mDSView);

	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(*mcWidth);
	mScreenViewport.Height = static_cast<float>(*mcHeight);
	mScreenViewport.MinDepth = 0.0f; // D3D uses a depth buffer range from 0
	mScreenViewport.MaxDepth = 1.0f; // ... to 1

	mDevCon->RSSetViewports(1, &mScreenViewport);
}

void Direct3D::Shutdown()
{
	mSwapChain->SetFullscreenState(false, nullptr);

	SafeRelease(mRTView);
	SafeRelease(mDSView);
	SafeRelease(mSwapChain);
	SafeRelease(mDSBuffer);

	if(mDevCon)
		mDevCon->ClearState();
	
	SafeRelease(mDevCon);
	SafeRelease(mDevice);
}