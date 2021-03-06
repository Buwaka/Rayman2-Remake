#include "stdafx.h"
#include "PostGrayscale.h"
#include "RenderTarget.h"

PostGrayscale::PostGrayscale()
	: PostProcessingMaterial(L"./Resources/Effects/PP/Grayscale.fx", 1),
	m_pTextureMapVariabele(nullptr)
{
}

void PostGrayscale::LoadEffectVariables()
{
	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariabele->IsValid())
	{
		Logger::LogWarning(L"LoadEffectVariables error!: gTexture <-> PostGrayScale");
	}
}

void PostGrayscale::UpdateEffectVariables(RenderTarget* pRendertarget, const GameContext&)
{
	m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
}