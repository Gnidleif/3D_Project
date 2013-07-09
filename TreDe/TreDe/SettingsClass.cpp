#include "SettingsClass.h"
using namespace std;

SettingsClass* SettingsClass::mInstance = nullptr;

SettingsClass* SettingsClass::GetInstance()
{
	if(!mInstance)
		mInstance = new SettingsClass();
	return mInstance;
}
// Reads the settings file and sets the data to the values in it
bool SettingsClass::Initialize()
{
	fstream in;
	in.open("../Data/Textfiles/Settings.txt");
	if(in.is_open())
	{
		mData = new SettingsData();
		string reader = "";

		getline(in, reader);
		this->mData->mWidth = atoi(reader.c_str());
		getline(in, reader);
		this->mData->mHeight = atoi(reader.c_str());
		getline(in, reader);
		this->mData->mIsFullscreen = (reader.at(0) == '1');
	}
	else
		return false;

	return true;
}
// Switch to fullscreen
void SettingsClass::SwitchFullscreen()
{
	if(mData->mIsFullscreen)
		mData->mIsFullscreen = false;
	else
		mData->mIsFullscreen = true;
}
// Sets the resolution to the values sent as parameters
void SettingsClass::SetResolution(unsigned int width, unsigned int height)
{
	mData->mWidth = width;
	mData->mHeight = height;
}
// This class doesn't include d3dutilities, just because I didn't want to include it all over the place
// Which is why it's not using the SafeDelete stuff
void SettingsClass::Shutdown()
{
	if(mData)
	{
		delete mData;
		mData = nullptr;
	}

	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

// Private functions

SettingsClass::SettingsClass()
	:
	mData(nullptr)
{
}