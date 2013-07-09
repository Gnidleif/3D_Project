#ifndef TEXTCLASS_H
#define TEXTCLASS_H
#include "d3dUtilities.h"
#include "DirectInput.h"
#include <FW1FontWrapper.h>
#include <string>
#include <map>

#define Text TextClass::GetInstance()

namespace TextColors
{
	const int Red		= 0xff0000ff;
	const int White		= 0xffffffff;
};

class TextClass
{
private:
	struct ConstantText
	{
		wchar_t* mText;
		float mPosX, mPosY;
		float mSize;
		int mColor;

		ConstantText()
			: mText(L""), mPosX(0.0f), mPosY(0.0f), mSize(0), mColor(0)
		{
		}

		ConstantText(wchar_t* text, float posX, float posY, float size, int color)
			: mText(text), mPosX(posX), mPosY(posY), mSize(size), mColor(color)
		{
		}
	};

	struct TimedText
	{
		wchar_t* mText;
		float mPosX, mPosY;
		float mTime, mSize;
		int mColor;

		TimedText()
			: mText(L""), mPosX(0.0f), mPosY(0.0f), mSize(0), mColor(0), mTime(0.0f)
		{
		}

		TimedText(wchar_t* text, float posX, float posY, float size, int color, float time)
			: mText(text), mPosX(posX), mPosY(posY), mSize(size), mColor(color), mTime(time)
		{
		}
	};

public:
	static TextClass* GetInstance();
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* devCon);
	void Shutdown();
	void ClearTexts();
	void Update(float dt);
	void Draw();

	void AddConstantText(std::string key, std::string text, float posX, float posY, float size, int color);
	void AddTimedText(std::string key, std::string text, float posX, float posY, float size, int color, float time);
	bool RemoveConstantText(std::string key);
	bool RemoveTimedText(std::string key);

	std::string IntToString(int toConvert);
	std::string FloatToString(float toConvert);
	std::string XMFLOAT3ToString(XMFLOAT3 toConvert);

public:
	UINT GetConstantSize() const { return this->mConstantTexts.size(); }
	UINT GetTimedSize() const { return this->mTimedTexts.size(); }

private:
	TextClass();
	TextClass(const TextClass&) {}
	void operator=(const TextClass&) {}
	~TextClass();
	void WriteText(wchar_t* text, float posX, float posY, float fontSize, int color) const;
	wchar_t* StringToWChar(std::string toConvert);

private:
	static TextClass* mInstance;

	IFW1Factory* mFW1Factory;
	IFW1FontWrapper* mFontWrapper;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDevCon;
	std::map<std::string, ConstantText> mConstantTexts;
	std::map<std::string, TimedText> mTimedTexts;
};
#endif

