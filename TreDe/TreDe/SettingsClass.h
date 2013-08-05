#pragma once
#ifndef SETTINGSCLASS_H
#define SETTINGSCLASS_H
#include <string>
#include <fstream>

#define Settings SettingsClass::GetInstance()
// Struct holding the current values of the relevant variables
struct SettingsData
{
	unsigned int mWidth, mHeight;
	bool mIsFullscreen;
};

// Singleton used to switch between resolutions/fullscreen of the program
// It's a singleton because the same instance of this object is used in the d3d11app/direct3d/main-classes
class SettingsClass
{
public: // Public functions
	static SettingsClass* GetInstance();
	bool Initialize();
	void SwitchFullscreen();
	void SetResolution(unsigned int width, unsigned int height);
	void Shutdown();
	
public: // Get functions
	SettingsData* GetData() const { return this->mData; }

private: // Private functions
	SettingsClass();
	SettingsClass(const SettingsClass&) {}
	void operator=(const SettingsClass&) {}
	~SettingsClass() {}

private: // Private variables
	static SettingsClass* mInstance;
	SettingsData* mData;
};
#endif