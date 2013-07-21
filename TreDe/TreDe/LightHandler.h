#ifndef LIGHTHANDLER_H
#define LIGHTHANDLER_H
#include "LightDef.h"
#include "Effects.h"

// Very hardcoded class used to handle them lights and stuff!

class LightHandler
{
public:
	LightHandler();
	~LightHandler();
	void Update(float dt);
	void ApplyEffects();

private:
	int mDirAmount;
	int mPointAmount;
	int mSpotAmount;

	DirectionalLight* mDirs;
	PointLight* mPoints;
	SpotLight* mSpots;
};
#endif