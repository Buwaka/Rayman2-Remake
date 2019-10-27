#include "stdafx.h"
#include "PostGrain.h"
#include "RenderTarget.h"


PostGrain::PostGrain(GrainCoordinate type , float strength, float scale):
	PostProcessingMaterial(L"./Resources/Effects/PP/Grain.fx", 1),
	m_pTextureMapVariabele(nullptr),
	m_pRandomNumberVariabele(nullptr),
	m_pStrengthVariabele(nullptr),
	m_pScaleVariabele(nullptr),
	m_Type(type),
	m_Strength(strength),
	m_Scale(scale),
	m_Updated(true)
{
}


void PostGrain::ChangeGrain(GrainCoordinate type, float strength, float scale)
{
	m_Type = type;
	m_Strength = strength;
	m_Scale = scale;
	m_Updated = true;
}

void PostGrain::LoadEffectVariables()
{
	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariabele->IsValid())
	{
		Logger::LogWarning(L"LoadEffectVariables error!: m_pTextureMapVariabele");
	}

	m_pRandomNumberVariabele = GetEffect()->GetVariableByName("gRandom")->AsScalar();
	if (!m_pRandomNumberVariabele->IsValid())
	{
		Logger::LogWarning(L"LoadEffectVariables error!: m_pRandomNumberVariabele");
	}

	m_pStrengthVariabele = GetEffect()->GetVariableByName("gStrength")->AsScalar();
	if (!m_pStrengthVariabele->IsValid())
	{
		Logger::LogWarning(L"LoadEffectVariables error!: m_pStrengthVariabele");
	}

	m_pScaleVariabele = GetEffect()->GetVariableByName("gScale")->AsScalar();
	if (!m_pScaleVariabele->IsValid())
	{
		Logger::LogWarning(L"LoadEffectVariables error!: m_pScaleVariabele");
	}
}

void PostGrain::UpdateEffectVariables(RenderTarget* pRendertarget, const GameContext& gameContext)
{

	if(m_pTextureMapVariabele)
		m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());

	if (m_pRandomNumberVariabele)
	{
		switch (m_Type)
		{
		case Time:
			m_pRandomNumberVariabele->SetFloat(gameContext.pGameTime->GetTotal());
			break;
		case Position:
			m_pRandomNumberVariabele->SetFloat(gameContext.pGameTime->GetTotal());
			break;
		default:
			m_pRandomNumberVariabele->SetFloat(gameContext.pGameTime->GetTotal());
			break;
		}
	}


	if (m_Updated)
	{
		if (m_pStrengthVariabele)
			m_pStrengthVariabele->SetFloat(m_Strength);

		if (m_pScaleVariabele)
			m_pScaleVariabele->SetFloat(m_Scale);

		m_Updated = false;
	}

}