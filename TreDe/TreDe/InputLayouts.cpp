#include "InputLayouts.h"
#include "Effects.h"

const D3D11_INPUT_ELEMENT_DESC InputLayouts::InputLayoutDesc::Pos[1] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayouts::InputLayoutDesc::PosNor[2] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayouts::InputLayoutDesc::PosNorTex[3] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayouts::InputLayoutDesc::PosNorTexTan[4] =
{
	{"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayouts::InputLayoutDesc::Particle[5] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"AGE", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

const D3D11_INPUT_ELEMENT_DESC InputLayouts::InputLayoutDesc::PosNorTexTanSkin[6] =
{
	{"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"WEIGHTS",		 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"BONEINDICES",  0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

// Initialization of inputlayouts

ID3D11InputLayout* InputLayouts::mPos = nullptr;
ID3D11InputLayout* InputLayouts::mPosNor = nullptr;
ID3D11InputLayout* InputLayouts::mPosNorTex = nullptr;
ID3D11InputLayout* InputLayouts::mPosNorTexTan = nullptr;
ID3D11InputLayout* InputLayouts::mParticle = nullptr;
ID3D11InputLayout* InputLayouts::mPosNorTexTanSkin = nullptr;

void InputLayouts::Initialize(ID3D11Device* device)
{
	D3DX11_PASS_DESC pd = {};
	// Skybox
	Effects::SkyFX->mSolid->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::Pos, 
		1, 
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPos);

	Effects::SkyFX->mWire->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::Pos, 
		1, 
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPos);

	// Terrain
	Effects::TerrainFX->mSolid->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTex,
		3,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTex);

	Effects::TerrainFX->mWire->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTex,
		3,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTex);

	Effects::TerrainFX->mAllLights->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTex,
		3,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTex);

	// Static normal
	Effects::NormalFX->mSolid->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTan,
		4,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTan);

	Effects::NormalFX->mSolidAlpha->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTan,
		4,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTan);

	Effects::NormalFX->mWire->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTan,
		4,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTan);

	Effects::NormalFX->mAllLights->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTan,
		4,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTan);

	Effects::NormalFX->mAllLightsAlpha->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTan,
		4,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTan);

	// Skinned normal 
	Effects::NormalFX->mSolidSkin->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTanSkin,
		6,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTanSkin);

	Effects::NormalFX->mSolidAlphaSkin->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTanSkin,
		6,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTanSkin);

	Effects::NormalFX->mWire->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTanSkin,
		6,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTanSkin);


	// Tessellation techniques
	Effects::TessFX->mSolid->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTan,
		4,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTan);

	Effects::TessFX->mSolidAlpha->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTan,
		4,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTan);

	Effects::TessFX->mWire->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTan,
		4,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTan);

	Effects::TessFX->mAllLights->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTan,
		4,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTan);

	Effects::TessFX->mAllLightsAlpha->GetPassByIndex(0)->GetDesc(&pd);
	device->CreateInputLayout(
		InputLayoutDesc::PosNorTexTan,
		4,
		pd.pIAInputSignature,
		pd.IAInputSignatureSize,
		&mPosNorTexTan);
}

void InputLayouts::Shutdown()
{
	SafeRelease(mPos);
	SafeRelease(mPosNor);
	SafeRelease(mPosNorTex);
	SafeRelease(mPosNorTexTan);
	SafeRelease(mParticle);
	SafeRelease(mPosNorTexTanSkin);
}