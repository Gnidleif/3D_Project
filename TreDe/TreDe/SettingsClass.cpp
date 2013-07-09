#include "SettingsClass.h"
using namespace std;

SettingsClass* SettingsClass::mInstance = nullptr;

SettingsClass* SettingsClass::GetInstance()
{
	if(!mInstance)
		mInstance = new SettingsClass();
	return mInstance;
}

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

void SettingsClass::SwitchFullscreen()
{
	if(mData->mIsFullscreen)
		mData->mIsFullscreen = false;
	else
		mData->mIsFullscreen = true;
}

void SettingsClass::SetResolution(unsigned int width, unsigned int height)
{
	mData->mWidth = width;
	mData->mHeight = height;
}

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