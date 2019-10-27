#pragma once
#include "Material.h"

class TextureData;

class DiffuseMaterial_Shadow_Panning final : public Material
{
public:
	DiffuseMaterial_Shadow_Panning();
	~DiffuseMaterial_Shadow_Panning() = default;

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetDiffuseTexture(TextureData* texture);
	void SetLightDirection(DirectX::XMFLOAT3 dir);

	void SetDirection(DirectX::XMFLOAT2 direction) { m_Direction = direction; };
	void SetSpeed(float speed) { m_Speed = speed; };

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	DirectX::XMFLOAT2 m_Direction;
	float m_Speed;
	TextureData* m_pDiffuseTexture;
	DirectX::XMFLOAT3 m_LightDirection = { -0.577f, -0.577f, 0.577f };

	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	static ID3DX11EffectShaderResourceVariable* m_pShadowSRVvariable;
	static ID3DX11EffectVectorVariable* m_pLightDirectionVariable;
	static ID3DX11EffectMatrixVariable* m_pLightWVPvariable;
	static ID3DX11EffectVectorVariable* m_pPanningVariabele;
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	DiffuseMaterial_Shadow_Panning(const DiffuseMaterial_Shadow_Panning &obj);
	DiffuseMaterial_Shadow_Panning& operator=(const DiffuseMaterial_Shadow_Panning& obj);
};

