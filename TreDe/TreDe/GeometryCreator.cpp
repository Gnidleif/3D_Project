#include "GeometryCreator.h"

void GeometryCreator::CreateSphere(float radius, UINT slice, UINT stack,  std::vector<VertexDef::PosNorTexTan>& vertices, std::vector<UINT>& indices)
{
	vertices.clear();
	indices.clear();

	VertexDef::PosNorTexTan top =
		VertexDef::PosNorTexTan(
		XMFLOAT3(0.0f, +radius, 0.0f), // Position
		XMFLOAT3(0.0f, +1.0f, 0.0f), // Normal
		XMFLOAT2(1.0f, 0.0f), // TexCoord
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)); // TangentU

	VertexDef::PosNorTexTan bottom =
		VertexDef::PosNorTexTan(
		XMFLOAT3(0.0f, -radius, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	vertices.push_back(top);

	float thetaStep = 2.0f*XM_PI/slice;
	float phiStep = XM_PI/stack;

	for(UINT i(1); i <= stack-1; ++i)
	{
		float phi = i*phiStep;
		for(UINT j(0); j <= slice; ++j)
		{
			float theta = j*thetaStep;

			VertexDef::PosNorTexTan v;
			v.mPos = XMFLOAT3(
				radius*sinf(phi)*cosf(theta),
				radius*cosf(phi),
				radius*sinf(phi)*sinf(theta));

			v.mTangentU = XMFLOAT4(
				-radius*sinf(phi)*sinf(theta),
				0.0f,
				+radius*sinf(phi)*cosf(theta),
				0.0f);

			XMVECTOR tangent = XMLoadFloat4(&v.mTangentU);
			XMStoreFloat4(&v.mTangentU, XMVector3Normalize(tangent));

			XMVECTOR position = XMLoadFloat3(&v.mPos);
			XMStoreFloat3(&v.mNormal, XMVector3Normalize(position));

			v.mTexCoord = XMFLOAT2(
				theta/XM_2PI,
				phi/XM_PI);

			vertices.push_back(v);
		}
	}

	vertices.push_back(bottom);

	for(UINT i(1); i <= slice; ++i)
	{
		indices.push_back(0);
		indices.push_back(i+1);
		indices.push_back(i);
	}

	UINT baseIndex(1);
	UINT ringVertexCount(slice+1);

	for(UINT i(0); i < stack-2; ++i)
	{
		for(UINT j(0); j < slice; ++j)
		{
			indices.push_back(baseIndex + i*ringVertexCount + j);
			indices.push_back(baseIndex + i*ringVertexCount + j+1);
			indices.push_back(baseIndex + (i+1)*ringVertexCount + j);

			indices.push_back(baseIndex + (i+1)*ringVertexCount + j);
			indices.push_back(baseIndex + i*ringVertexCount + j+1);
			indices.push_back(baseIndex + (i+1)*ringVertexCount + j+1);
		}
	}

	UINT southPoleIndex(vertices.size()-1);
	baseIndex = southPoleIndex-ringVertexCount;

	for(UINT i(0); i < slice; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex+i);
		indices.push_back(baseIndex+i+1);
	}
}