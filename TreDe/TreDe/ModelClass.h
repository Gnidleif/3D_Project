#ifndef MODELCLASS_H
#define MODELCLASS_H
#include "TerrainModel.h"
#include "StaticModel.h"
#include "SkinnedModel.h"
#include <map>

#define Model ModelClass::GetInstance()
// Singleton used to hold the various 3D-objects in the program
// Used to avoid the same type of model being created more than once
class ModelClass
{
public:
	static ModelClass* GetInstance();
	void Shutdown();
	void Initialize(ID3D11Device* device);
	TerrainModel* GetTerrainModel(std::string key);
	StaticModel* GetStaticModel(std::string key, std::string texPath);
	SkinnedModel* GetSkinnedModel(std::string key, std::string texPath);

private:
	static ModelClass* mInstance;
	// These two <map>s contains the actual models, the key is the filepath
	std::map<std::string, TerrainModel*> mTerrainModels;
	std::map<std::string, StaticModel*> mStaticModels;
	std::map<std::string, SkinnedModel*> mSkinnedModels;
	ID3D11Device* mDevice;

private:
	ModelClass() {}
	ModelClass(const ModelClass&) {}
	void operator=(const ModelClass&) {}
	~ModelClass() {}
};
#endif