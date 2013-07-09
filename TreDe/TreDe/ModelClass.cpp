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
	for(auto& it(mTerrainModels.begin()); it != mTerrainModels.end(); ++it)
	{
		SafeDelete(it->second);
	}
	mTerrainModels.clear();

	for(auto& it(mStaticModels.begin()); it != mStaticModels.end(); ++it)
	{
		SafeDelete(it->second);
	}

	SafeDelete(mInstance);
}

TerrainModel* ModelClass::GetTerrainModel(std::string key)
{
	TerrainModel* tm = nullptr;
	if(mTerrainModels[key])
	{
		tm = mTerrainModels[key];
	}
	else
	{
		tm = new TerrainModel(key);
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
		sm = new StaticModel(key, texPath);
		mStaticModels[key] = sm;
	}
	return sm;
}
