#pragma once
#include "PostProcessingMaterial.h"



class ID3D11EffectShaderResourceVariable;

class PostGrain : public PostProcessingMaterial
{
public:
	enum GrainCoordinate
	{
		Time,
		Position
	};

	PostGrain(GrainCoordinate type = Time, float strength = 0.1f, float scale = 1000.0f);
	PostGrain(const PostGrain& other) = delete;
	PostGrain(PostGrain&& other) noexcept = delete;
	PostGrain& operator=(const PostGrain& other) = delete;
	PostGrain& operator=(PostGrain&& other) noexcept = delete;
	virtual ~PostGrain() = default;

	void ChangeGrain(GrainCoordinate type = Time, float strength = 1.0f, float scale = 1.0f);

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget, const GameContext& gameContext) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
	ID3DX11EffectScalarVariable* m_pRandomNumberVariabele;
	ID3DX11EffectScalarVariable* m_pStrengthVariabele;
	ID3DX11EffectScalarVariable* m_pScaleVariabele;
	GrainCoordinate m_Type;
	float m_Strength, m_Scale;
	bool m_Updated;
};
