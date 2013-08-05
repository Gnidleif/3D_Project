#pragma once
#ifndef TEXTURECLASS_H
#define TEXTURECLASS_H
#include "d3dUtilities.h"
#include <map>

#define Texture TextureClass::GetInstance()
// This does the same stuff and works the same way as the ModelClass,
// except for textures (Y)
class TextureClass
{
public:
	static TextureClass* GetInstance();
	void Initialize(ID3D11Device* device);
	void Shutdown();

public:
	ID3D11ShaderResourceView* GetTexture(std::string key);

private:
	TextureClass();
	TextureClass(TextureClass const&) {}
	void operator=(TextureClass const&) {}
	~TextureClass() {}

private:
	static TextureClass* mInstance;
	ID3D11Device* mDevice;
	std::map<std::string, ID3D11ShaderResourceView*> mTextureSRV;
};
#endif