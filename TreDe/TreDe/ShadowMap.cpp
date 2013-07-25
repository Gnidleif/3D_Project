#include "ShadowMap.h"

ShadowMap::ShadowMap(void)
	:
	mDevice(nullptr), mWidth(0), mHeight(0),
	mDepthMapSRV(nullptr), mDepthMapDSV(nullptr)
{
	XMLoadFloat4x4(&mLightView);
	XMLoadFloat4x4(&mLightProj);
}

ShadowMap::~ShadowMap(void)
{
	SafeRelease(mDepthMapSRV);
	SafeRelease(mDepthMapDSV);
}

void ShadowMap::Initialize(ID3D11Device* device)
{
	this->mDevice = device;
	this->UpdateResolution();
	this->CreateShadowmap();
}

void ShadowMap::BindDSVAndSetRenderTarget(ID3D11DeviceContext* devCon)
{
	devCon->RSSetViewports(1, &mViewport);
	ID3D11RenderTargetView* renderTargets[1] = {0};
	devCon->OMSetRenderTargets(1, renderTargets, mDepthMapDSV);
	devCon->ClearDepthStencilView(mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowMap::UpdateResolution()
{
	SafeRelease(mDepthMapSRV);
	SafeRelease(mDepthMapDSV);
	mWidth = Settings->GetData()->mWidth;
	mHeight = Settings->GetData()->mHeight;
}

void ShadowMap::Draw(ID3D11DeviceContext* devCon, 
					 ID3DX11EffectTechnique* activeTech, 
					 std::vector<StaticEntity*> entities, 
					 PointLight* light, 
					 Camera* camera)
{
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devCon->IASetInputLayout(InputLayouts::mPosNorTexTan);

	D3DX11_TECHNIQUE_DESC techDesc = {};
	activeTech->GetDesc(&techDesc);

	//Effects::ShadowFX->SetView(XMLoadFloat4x4(&light->View));
	//Effects::ShadowFX->SetProj(XMLoadFloat4x4(&light->Proj));
	Effects::ShadowFX->SetEyePos(camera->GetPosition());

	for(UINT i(0); i != techDesc.Passes; ++i)
	{
		for(UINT j(0); j != entities.size(); ++j)
		{
			Effects::ShadowFX->SetWorld(XMLoadFloat4x4(&entities[j]->GetModelInstance()->mWorld));
			Effects::ShadowFX->SetWorldInvTranspose(MathHelper::InverseTranspose(XMLoadFloat4x4(&entities[j]->GetModelInstance()->mWorld)));

			for(UINT k(0); k != entities[j]->GetModelInstance()->mModel->GetMeshCount(); ++k)
			{
				UINT index = entities[j]->GetModelInstance()->mModel->GetMesh(k)->GetMaterialIndex();
				entities[j]->GetModelInstance()->mModel->GetDiffMapSRV(index);
				entities[j]->GetModelInstance()->mModel->GetNormalMapSRV(index);

				activeTech->GetPassByIndex(i)->Apply(0, devCon);
				entities[j]->GetModelInstance()->mModel->GetMesh(k)->Draw(devCon);
			}
		}
	}
}

void ShadowMap::CreateShadowmap()
{
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(mWidth);
	mViewport.Height = static_cast<float>(mHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* depthMap = nullptr;
	mDevice->CreateTexture2D(&texDesc, 0, &depthMap);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	mDevice->CreateDepthStencilView(depthMap, &dsvDesc, &mDepthMapDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	mDevice->CreateShaderResourceView(depthMap, &srvDesc, &mDepthMapSRV);

	SafeRelease(depthMap);
}
