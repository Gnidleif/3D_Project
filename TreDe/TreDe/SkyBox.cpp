#include "SkyBox.h"

SkyBox::SkyBox(void)
	:
	mVertexBuffer(nullptr),
	mIndexBuffer(nullptr),
	mCubeMapSRV(nullptr),
	mFileName("../Data/Textures/SkyBox_Space.dds")
{
}

SkyBox::~SkyBox(void)
{
	SafeRelease(mVertexBuffer);
	SafeRelease(mIndexBuffer);
	SafeRelease(mCubeMapSRV);
}

void SkyBox::Initialize(ID3D11Device* device, float radius)
{
	this->mCubeMapSRV = Texture->GetTexture(mFileName);
	std::vector<VertexDef::PosNorTexTan> sphere;
	GeometryCreator::CreateSphere(radius, 30, 30, sphere, mIndices);

	std::vector<XMFLOAT3> positions(sphere.size());

	for(UINT i(0); i != sphere.size(); ++i)
		positions[i] = sphere[i].mPos;

	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(typeid(positions[0])) * positions.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData = {};
	vinitData.pSysMem = &positions[0];
	device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer);

	mIndexCount = mIndices.size();

	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = &mIndices[0];
	device->CreateBuffer(&ibd, &iinitData, &mIndexBuffer);
}

void SkyBox::Draw(ID3D11DeviceContext* devCon, ID3DX11EffectTechnique* activeTech, Camera* camera)
{
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	devCon->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	devCon->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	devCon->IASetInputLayout(InputLayouts::mPos);
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMFLOAT3 eye = camera->GetPosition();
	XMMATRIX T = XMMatrixTranslation(eye.x, eye.y, eye.z);
	XMMATRIX WVP = XMMatrixMultiply(T, camera->GetViewProjMatrix());

	Effects::SkyFX->SetWVP(&WVP);
	Effects::SkyFX->SetCubeMap(mCubeMapSRV);

	D3DX11_TECHNIQUE_DESC td = {};
	activeTech->GetDesc(&td);

	for(UINT i(0); i < td.Passes; ++i)
	{
		ID3DX11EffectPass* pass = Effects::SkyFX->mSkyTech->GetPassByIndex(i);
		pass->Apply(0, devCon);
	}
	devCon->DrawIndexed(mIndexCount, 0, 0);
}