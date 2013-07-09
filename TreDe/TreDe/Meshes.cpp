#include "Meshes.h"

// Functions for the virtual class

VirtualMesh::VirtualMesh()
	:
	mOffset(0),
	mIndices(std::vector<UINT>()),
	mVertexBuffer(nullptr),
	mIndexBuffer(nullptr),
	mIndexBufferFormat(DXGI_FORMAT_R32_UINT),
	mVertexStride(0),
	mMaterialIndex(0)
{
}

VirtualMesh::~VirtualMesh()
{
	SafeRelease(mVertexBuffer);
	SafeRelease(mIndexBuffer);
}

void VirtualMesh::Draw(ID3D11DeviceContext* devCon)
{
	devCon->IASetVertexBuffers(0, 1, &mVertexBuffer, &mVertexStride, &mOffset);
	devCon->IASetIndexBuffer(mIndexBuffer, mIndexBufferFormat, 0);

	devCon->DrawIndexed(mIndices.size(), 0, 0);
}

void VirtualMesh::SetIndexBuffer(ID3D11Device* device, const UINT* indice, UINT count)
{
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * count;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iiData = {};
	iiData.pSysMem = indice;
	device->CreateBuffer(&ibd, &iiData, &mIndexBuffer);
}