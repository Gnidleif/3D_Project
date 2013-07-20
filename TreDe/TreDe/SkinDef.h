#ifndef SKINDEF_H
#define SKINDEF_H
#include "d3dUtilities.h"
#include <assimp/scene.h>

namespace SkinDef
{
	struct VertexWeight
	{
		unsigned int mID;
		float mWeight;

		VertexWeight()
			: mID(0), mWeight(0.0f) {}
	};

	struct Bone
	{
		std::string mName;
		XMFLOAT4X4 mOffset;
		XMFLOAT4X4 mLocalTrans;
		XMFLOAT4X4 mGlobalTrans;
		XMFLOAT4X4 mOrigLocalTrans;
		std::vector<SkinDef::VertexWeight> mWeights;

		SkinDef::Bone* mParent;
		std::vector<Bone*> mChildren;

		Bone()
			: mName(""), mParent(nullptr)
		{
			XMStoreFloat4x4(&this->mOffset, XMMatrixIdentity());
			XMStoreFloat4x4(&this->mLocalTrans, XMMatrixIdentity());
			XMStoreFloat4x4(&this->mGlobalTrans, XMMatrixIdentity());
			XMStoreFloat4x4(&this->mOrigLocalTrans, XMMatrixIdentity());
		}

		~Bone()
		{
			for(auto& it(mChildren.begin()); it != mChildren.end(); ++it)
			{
				SafeDelete(*it);
			}
			mChildren.clear();
		}

		
	};

	struct AnimChannel
	{
		std::string mName;
		std::vector<aiVectorKey> mPosKeys;
		std::vector<aiQuatKey> mRotKeys;
		std::vector<aiVectorKey> mScaleKeys;
	};

	static XMFLOAT4X4 ReadAiMatrix(aiMatrix4x4& in)
	{
		XMFLOAT4X4 out;
		XMStoreFloat4x4(&out, XMMatrixIdentity());
		out._11 = in.a1;
		out._12 = in.a2;
		out._13 = in.a3;
		out._14 = in.a4;

		out._21 = in.b1;
		out._22 = in.b2;
		out._23 = in.b3;
		out._24 = in.b4;

		out._31 = in.c1;
		out._32 = in.c2;
		out._33 = in.c3;
		out._34 = in.c4;

		out._41 = in.d1;
		out._42 = in.d2;
		out._43 = in.d3;
		out._44 = in.d4;

		return out;
	}

	static void CalculateBoneToWorld(SkinDef::Bone* &child)
	{
		child->mGlobalTrans = child->mLocalTrans;
		XMMATRIX Child_GlobalTrans = XMLoadFloat4x4(&child->mGlobalTrans);
		SkinDef::Bone* parent = child->mParent;

		while(parent)
		{
			XMMATRIX Parent_LocalTrans = XMLoadFloat4x4(&parent->mLocalTrans);
			Child_GlobalTrans = XMMatrixMultiply(Child_GlobalTrans, Parent_LocalTrans);
			parent = parent->mParent;
		}
		XMStoreFloat4x4(&child->mGlobalTrans, Child_GlobalTrans);
	}
}
#endif