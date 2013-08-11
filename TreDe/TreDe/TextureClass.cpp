#include "TextureClass.h"

// Static singleton-y stuff
TextureClass* TextureClass::mInstance = nullptr;

TextureClass* TextureClass::GetInstance()
{
	if(!mInstance)
		mInstance = new TextureClass();
	return mInstance;
}

// Public functions
void TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* devCon)
{
	this->mDevice = device;
	this->mDevCon = devCon;
}

void TextureClass::Shutdown()
{
	for(auto& it(mTextureSRV.begin()); it != mTextureSRV.end(); ++it)
		SafeRelease(it->second);
	mTextureSRV.clear();
	SafeDelete(mInstance);
}

ID3D11ShaderResourceView* TextureClass::GetTexture(std::string key)
{
	ID3D11ShaderResourceView* srv = nullptr;
	if(!(srv = mTextureSRV[key]))
	{
		D3DX11CreateShaderResourceViewFromFile(mDevice, key.c_str(), 0, 0, &srv, 0);
	}
	return srv;
}

ID3D11ShaderResourceView* TextureClass::GetTexArray(std::string key)
{
	ID3D11ShaderResourceView* srv = nullptr;
	if(!(srv = mTextureSRV[key]))
	{
		std::vector<LPCSTR> path;
		path.push_back(key.c_str());
		srv = d3dHelper::CreateTexture2DArraySRV(mDevice, mDevCon, path);
	}
	return srv;
}

ID3D11ShaderResourceView* TextureClass::GetRandomTex()
{
	return d3dHelper::CreateRandomTexture1DSRV(this->mDevice);
}

// Private functions
TextureClass::TextureClass(void)
	: 
	mDevice(nullptr),
	mDevCon(nullptr)
{
}