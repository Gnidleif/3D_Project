#include "ModelClass.h"
using namespace std;

ModelClass* ModelClass::mInstance = nullptr;

ModelClass* ModelClass::GetInstance()
{
	if(!mInstance)
		mInstance = new ModelClass();
	return mInstance;
}
//

void ModelClass::Shutdown()
{
	for(auto& it(mTerrainModels.begin()); it != mTerrainModels.end(); ++it) // Magic
	{
		SafeDelete(it->second);
	}
	mTerrainModels.clear();

	for(auto& it(mStaticModels.begin()); it != mStaticModels.end(); ++it)
	{
		SafeDelete(it->second);
	}
	mStaticModels.clear();

	for(auto& it(mSkinnedModels.begin()); it != mSkinnedModels.end(); ++it)
	{
		SafeDelete(it->second);
	}
	mSkinnedModels.clear();

	SafeDelete(mInstance);
}

void ModelClass::Initialize(ID3D11Device* device)
{
	mDevice = device;
}

TerrainModel* ModelClass::GetTerrainModel(std::string key)
{
	TerrainModel* tm = nullptr;
	if(mTerrainModels[key]) // If the object at the key exists, return it
	{
		tm = mTerrainModels[key];
	}
	else // Otherwise create it and return the new object
	{
		tm = new TerrainModel(mDevice, key);
		mTerrainModels[key] = tm;
	}
	return tm;
}

StaticModel* ModelClass::GetStaticModel(std::string key, std::string texPath)
{
	StaticModel* sm = nullptr;
	if(mStaticModels[key])
	{
		sm = mStaticModels[key];
	}
	else
	{
		sm = new StaticModel(mDevice, key, texPath);
		mStaticModels[key] = sm;
	}
	return sm;
}

SkinnedModel* ModelClass::GetSkinnedModel(std::string key, std::string texPath)
{
	SkinnedModel* sm = nullptr;
	if(mSkinnedModels[key])
	{
		sm = mSkinnedModels[key];
	}
	else
	{
		sm = new SkinnedModel(mDevice, key, texPath);
		mSkinnedModels[key] = sm;
	}
	return sm;
}