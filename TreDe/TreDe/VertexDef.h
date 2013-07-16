#ifndef VERTEXDEF_H
#define VERTEXDEF_H
#include "d3dUtilities.h"

namespace VertexDef
{
	struct Pos
	{
		XMFLOAT3 mPos;
		Pos() {}
		Pos(XMFLOAT3 pos)
			: mPos(pos) 
		{
			ZeroMemory(this, sizeof(this));
		}
	};

	struct PosNor
	{
		XMFLOAT3 mPos;
		XMFLOAT3 mNormal;
		PosNor() {}
		PosNor(XMFLOAT3 pos, XMFLOAT3 normal)
			: mPos(pos), mNormal(normal) {}
	};

	struct PosNorTex
	{
		XMFLOAT3 mPos;
		XMFLOAT3 mNormal;
		XMFLOAT2 mTexCoord;
		PosNorTex() {}
		PosNorTex(XMFLOAT3 pos, XMFLOAT3 normal, XMFLOAT2 texCoord)
			: mPos(pos), mNormal(normal), mTexCoord(texCoord) {}
	};

	struct PosNorTexTan
	{
		XMFLOAT3 mPos;
		XMFLOAT3 mNormal;
		XMFLOAT2 mTexCoord;
		XMFLOAT4 mTangentU;
		PosNorTexTan() {}
		PosNorTexTan(XMFLOAT3 pos, XMFLOAT3 normal, XMFLOAT2 texCoord, XMFLOAT4 tangentU)
			: mPos(pos), mNormal(normal), mTexCoord(texCoord), mTangentU(tangentU) {}
	};

	struct PosNorTexTanSkin
	{
		XMFLOAT3 mPos;
		XMFLOAT3 mNormal;
		XMFLOAT2 mTexCoord;
		XMFLOAT4 mTangentU;
		XMFLOAT3 mWeights;
		BYTE mBoneIndices[4];
		PosNorTexTanSkin() {}
		PosNorTexTanSkin(XMFLOAT3 pos, XMFLOAT3 normal, XMFLOAT2 texCoord, XMFLOAT4 tangentU, XMFLOAT3 weights, BYTE* boneIndices)
			: mPos(pos), mNormal(normal), mTexCoord(texCoord), mTangentU(tangentU), mWeights(weights)
		{
			for(UINT i(0); i != 4; ++i)
			{
				mBoneIndices[i] = boneIndices[i];
			}
		}
	};

	struct Particle
	{
		XMFLOAT3 mInitPos;
		XMFLOAT3 mInitVel;
		XMFLOAT2 mSize;
		float mAge;
		UINT mType;
		Particle() {}
		Particle(XMFLOAT3 initPos, XMFLOAT3 initVel, XMFLOAT2 size, float age, UINT type)
			: mInitPos(initPos), mInitVel(initVel), mSize(size), mAge(age), mType(type) {}
	};
};
#endif