#ifndef MODELCLASS_H
#define MODELCLASS_H
#include "TerrainModel.h"
#include "StaticModel.h"
#include <map>

#define Model ModelClass::GetInstance()

class ModelClass
{
public:
	static ModelClass* GetInstance();
	void Shutdown();
	TerrainModel* GetTerrainModel(std::string key);
	StaticModel* GetStaticModel(std::string key, std::string texPath);

private:
	static ModelClass* mInstance;
	ID3D11Device* mDevice;
	std::map<std::string, TerrainModel*> mTerrainModels;
	std::map<std::string, StaticModel*> mStaticModels;

private:
	ModelClass() {}
	ModelClass(const ModelClass&) {}
	void operator=(const ModelClass&) {}
	~ModelClass() {}
};
#endif