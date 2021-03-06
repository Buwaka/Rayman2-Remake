#pragma once
#include "Material.h"

class TextureData;

class DiffuseMaterial : public Material
{
public:
	DiffuseMaterial();
	~DiffuseMaterial();

	void SetDiffuseTexture(const std::wstring &assetFile);
	void SetDiffuseTexture(TextureData* texture);

	Material* Clone() override;

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	TextureData* m_pDiffuseTexture;
	ID3DX11EffectShaderResourceVariable *m_pDiffuseSRVvariable;

	DiffuseMaterial(const DiffuseMaterial &obj) = delete;
	DiffuseMaterial& operator=(const DiffuseMaterial &obj) = delete;
};

