#include "TextClass.h"
#include <sstream>

TextClass* TextClass::mInstance = nullptr;

// Public functions
TextClass* TextClass::GetInstance()
{
	if(!mInstance)
		mInstance = new TextClass();
	return mInstance;
}

void TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* devCon)
{
	this->mDevice = device;
	this->mDevCon = devCon;

	FW1CreateFactory(FW1_VERSION, &mFW1Factory);
	mFW1Factory->CreateFontWrapper(mDevice, L"Lucida Console", &mFontWrapper);
}

void TextClass::Shutdown()
{
	ClearTexts();

	SafeRelease(mFW1Factory);
	SafeRelease(mFontWrapper);
	SafeDelete(mInstance);
}

void TextClass::ClearTexts()
{
	for(auto& it(mConstantTexts.begin()); it != mConstantTexts.end(); ++it)
	{
		SafeDelete(it->second.mText);
	}
	mConstantTexts.clear();

	for(auto& it(mTimedTexts.begin()); it != mTimedTexts.end(); ++it)
	{
		SafeDelete(it->second.mText);
	}
	mTimedTexts.clear();
}

void TextClass::Update(float dt)
{
	for(auto& it(mTimedTexts.begin()); it != mTimedTexts.end(); ++it)
	{
		it->second.mTime -= dt;
		if(it->second.mTime <= 0)
		{
			SafeDelete(it->second.mText);
			mTimedTexts.erase(it);
			break;
		}
	}
}

void TextClass::Draw()
{
	for(auto& it(mConstantTexts.begin()); it != mConstantTexts.end(); ++it)
	{
		this->WriteText(it->second.mText, it->second.mPosX, it->second.mPosY, it->second.mSize, it->second.mColor);
	}

	for(auto& it(mTimedTexts.begin()); it != mTimedTexts.end(); ++it)
	{
		this->WriteText(it->second.mText, it->second.mPosX, it->second.mPosY, it->second.mSize, it->second.mColor);
	}
}

void TextClass::AddConstantText(std::string key, std::string text, float posX, float posY, float size, int color)
{
	if(mConstantTexts.find(key) != mConstantTexts.end())
	{
		this->RemoveConstantText(key);
	}
	this->mConstantTexts[key] = ConstantText(this->StringToWChar(text), posX, posY, size, color);
}

void TextClass::AddTimedText(std::string key, std::string text, float posX, float posY, float size, int color, float time)
{
	if(mTimedTexts.find(key) != mTimedTexts.end())
	{
		this->RemoveTimedText(key);
	}
	this->mTimedTexts[key] = TimedText(this->StringToWChar(text), posX, posY, size, color, time);
}

bool TextClass::RemoveConstantText(std::string key)
{
	for(auto& it(mConstantTexts.begin()); it != mConstantTexts.end(); ++it)
	{
		if(it->first == key)
		{
			SafeDelete(it->second.mText);
			mConstantTexts.erase(it);
			return true;
		}
	}
	return false;
}

bool TextClass::RemoveTimedText(std::string key)
{
	for(auto& it(mTimedTexts.begin()); it != mTimedTexts.end(); ++it)
	{
		if(it->first == key)
		{
			SafeDelete(it->second.mText);
			mTimedTexts.erase(it);
			return true;
		}
	}
	return false;
}

std::string TextClass::IntToString(int toConvert)
{
	std::stringstream sText;
	sText << toConvert;
	return sText.str();
}

std::string TextClass::FloatToString(float toConvert)
{
	std::stringstream sText;
	sText << toConvert;
	return sText.str();
}

std::string TextClass::XMFLOAT3ToString(XMFLOAT3 toConvert)
{
	return "( " +
		FloatToString(toConvert.x) + ", " +
		FloatToString(toConvert.y) + ", " +
		FloatToString(toConvert.z) + " )";
}

// Private functions
TextClass::TextClass(void)
	:
	mFW1Factory(nullptr),
	mFontWrapper(nullptr),
	mDevice(nullptr),
	mDevCon(nullptr)
{
}

TextClass::~TextClass(void)
{
}

wchar_t* TextClass::StringToWChar(std::string toConvert) // Warning: wText needs to be deleted later since it calls new
{
	size_t strLength = strlen(toConvert.c_str()) + 1;
	wchar_t* wText = new wchar_t[strLength];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wText, strLength, toConvert.c_str(), strLength);

	return wText;
}

void TextClass::WriteText(wchar_t* text, float posX, float posY, float fontSize, int color) const
{
	mFontWrapper->DrawString(
	mDevCon,
	text,
	fontSize,
	posX,
	posY,
	color,
	0);
}