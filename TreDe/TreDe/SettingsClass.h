#ifndef SETTINGSCLASS_H
#define SETTINGSCLASS_H
#include <string>
#include <fstream>

#define Settings SettingsClass::GetInstance()

struct SettingsData
{
	unsigned int mWidth, mHeight;
	bool mIsFullscreen;
};


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