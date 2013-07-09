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
void TextureClass::Initialize(ID3D11Device* device)
{
	this->mDevice = device;
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

// Private functions
TextureClass::TextureClass(void)
	: 
	mDevice(nullptr)
{
}