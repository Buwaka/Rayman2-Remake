#pragma once
#include "ContentLoader.h"
#include "SpriteFont.h"

class BinaryReader;

class SpriteFontLoader : public ContentLoader<SpriteFont>
{
public:
	SpriteFontLoader() = default;
	virtual ~SpriteFontLoader() = default;

	SpriteFontLoader(const SpriteFontLoader& other) = delete;
	SpriteFontLoader(SpriteFontLoader&& other) noexcept = delete;
	SpriteFontLoader& operator=(const SpriteFontLoader& other) = delete;
	SpriteFontLoader& operator=(SpriteFontLoader&& other) noexcept = delete;

protected:
	SpriteFont* LoadContent(const std::wstring& assetFile) override;
	void Destroy(SpriteFont* objToDestroy) override;

	void InfoBlock(BinaryReader* pBinReader, SpriteFont* pSpriteFont);
	void CommonBlock(BinaryReader* pBinReader, SpriteFont* pSpriteFont);
	void PagesBlock(BinaryReader* pBinReader, SpriteFont* pSpriteFont, std::wstring assetFile);
	void CharsBlock(BinaryReader* pBinReader, SpriteFont* pSpriteFont);
};
