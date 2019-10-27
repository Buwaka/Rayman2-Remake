#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "BinaryReader.h"
#include "ContentManager.h"
#include "TextureData.h"


SpriteFont* SpriteFontLoader::LoadContent(const std::wstring& assetFile)
{
	auto pBinReader = new BinaryReader();
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	std::string idBytes{};
	for (size_t i = 0; i < 4; i++)
	{
		idBytes += pBinReader->Read<char>();
	}
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	auto temp = strcmp(idBytes.c_str(), "BMF");
	if (temp <= 0)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		return nullptr;
	}

	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	char version = idBytes[3];
	if (version < 3)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
		return nullptr;
	}

	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)

	/*
	while (eof())
	{
	blockId = static_cast<int>(pBinReader->Read<char>());
	blockSize = pBinReader->Read<uint32_t>();
	switch(blockId)
	{
	case info:
	break;
	...
	}
	}

	*/
	int blockId = static_cast<int>(pBinReader->Read<char>());
	InfoBlock(pBinReader, pSpriteFont);

	blockId = static_cast<int>(pBinReader->Read<char>());
	CommonBlock(pBinReader, pSpriteFont);

	blockId = static_cast<int>(pBinReader->Read<char>());
	PagesBlock(pBinReader, pSpriteFont, assetFile);

	blockId = static_cast<int>(pBinReader->Read<char>());
	CharsBlock(pBinReader, pSpriteFont);

	
	//DONE :)

	delete pBinReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}

void SpriteFontLoader::InfoBlock(BinaryReader* pBinReader, SpriteFont* pSpriteFont)
{
	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	uint32_t blockSize = pBinReader->Read<uint32_t>();
	UNREFERENCED_PARAMETER(blockSize);
	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	uint16_t fontSize = pBinReader->Read<uint16_t>();
	pSpriteFont->m_FontSize = fontSize;

	//Move the binreader to the start of the FontName[BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	pBinReader->MoveBufferPosition(12);

	//Retrieve the FontName [SpriteFont::m_FontName]
	std::wstring name{};
	name = pBinReader->ReadNullString();
	pSpriteFont->m_FontName = name;
}

void SpriteFontLoader::CommonBlock(BinaryReader * pBinReader, SpriteFont * pSpriteFont)
{
	//**********
// BLOCK 1 *
//**********
//Retrieve the blockId and blockSize
	uint32_t blockSize = pBinReader->Read<uint32_t>();
	UNREFERENCED_PARAMETER(blockSize);
	//move 4 pos, scale w & scale h
	pBinReader->MoveBufferPosition(4);

	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	uint16_t textureWidth = pBinReader->Read<uint16_t>();
	uint16_t textureHeight = pBinReader->Read<uint16_t>();
	pSpriteFont->m_TextureWidth = textureWidth;
	pSpriteFont->m_TextureHeight = textureHeight;

	//Retrieve PageCount
	//> if pagecount > 1
	//> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed)
	uint16_t pageCount = pBinReader->Read<uint16_t>();

	if (pageCount > 1)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed");
	}

	//move 5 pos to the next block
	pBinReader->MoveBufferPosition(5);
}

void SpriteFontLoader::PagesBlock(BinaryReader * pBinReader, SpriteFont * pSpriteFont, std::wstring assetFile)
{

	//**********
	// BLOCK 2 *
	//**********


	//Retrieve blockSize
	uint32_t blockSize = pBinReader->Read<uint32_t>();
	UNREFERENCED_PARAMETER(blockSize);
	//Retrieve the PageName (store Local)
	std::wstring pageName{};
	pageName = pBinReader->ReadNullString();

	//	> If PageName is empty
	//	> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty])
	//>Retrieve texture filepath from the assetFile path
	if (pageName.empty())
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]");
	}

	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)

	pageName = assetFile.substr(0, assetFile.rfind('/') + 1) + pageName;
	pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(pageName);

}

void SpriteFontLoader::CharsBlock(BinaryReader * pBinReader, SpriteFont * pSpriteFont)
{

	//**********
		// BLOCK 3 *
		//**********
		//Retrieve the blockId and blockSize

	uint32_t blockSize = pBinReader->Read<uint32_t>();

	//Retrieve Character Count (see documentation)
	const int numCharsBlocks = blockSize / 20;

	//Retrieve Every Character, For every Character:
	for (int i = 0; i < numCharsBlocks; i++)
	{
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		wchar_t charId = (wchar_t)pBinReader->Read<uint32_t>();

		//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
		if (!SpriteFont::IsCharValid(charId))
		{
			Logger::LogWarning(L"Character not valid");
			continue;
		}

		//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
		FontMetric& fontMetric = pSpriteFont->GetMetric(charId);

		//> Set IsValid to true [FontMetric::IsValid]
		fontMetric.IsValid = true;

		//> Set Character (CharacterId) [FontMetric::Character]
		fontMetric.Character = charId;

		//> Retrieve Xposition (store Local)
		uint16_t charPosX = pBinReader->Read<uint16_t>();

		//> Retrieve Yposition (store Local)
		uint16_t charPosY = pBinReader->Read<uint16_t>();

		//> Retrieve & Set Width [FontMetric::Width]
		fontMetric.Width = pBinReader->Read<uint16_t>();

		//> Retrieve & Set Height [FontMetric::Height]
		fontMetric.Height = pBinReader->Read<uint16_t>();

		//> Retrieve & Set OffsetX [FontMetric::OffsetX]
		fontMetric.OffsetX = pBinReader->Read<uint16_t>();

		//> Retrieve & Set OffsetY [FontMetric::OffsetY]
		fontMetric.OffsetY = pBinReader->Read<uint16_t>();

		//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
		fontMetric.AdvanceX = pBinReader->Read<uint16_t>();

		//> Retrieve & Set Page [FontMetric::Page]
		fontMetric.Page = pBinReader->Read<uint8_t>();

		//> Retrieve Channel (BITFIELD!!!) 
		//	> See documentation for BitField meaning [FontMetrix::Channel]
		uint8_t channel = pBinReader->Read<uint8_t>();
		//blue
		if ((channel & 0b00000001) > 0)
		{
			fontMetric.Channel += 1;
		}
		//green
		if ((channel & 0b00000010) > 0)
		{
			fontMetric.Channel += 2;
		}
		//red
		if ((channel & 0b00000100) > 0)
		{
			fontMetric.Channel += 4;
		}
		//alpha
		if ((channel & 0b00001000) > 0)
		{
			fontMetric.Channel += 8;
		}

		//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
		fontMetric.TexCoord = DirectX::XMFLOAT2(charPosX / float(pSpriteFont->m_TextureWidth), charPosY / float(pSpriteFont->m_TextureHeight));
	}
}
