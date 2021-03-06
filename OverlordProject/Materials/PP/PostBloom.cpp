#include "stdafx.h"
#include "PostBloom.h"
#include "RenderTarget.h"

PostBloom::PostBloom()
	: PostProcessingMaterial(L"./Resources/Effects/PP/Bloom.fx",3),
	m_pTextureMapVariabele(nullptr)
{
}

void PostBloom::LoadEffectVariables()
{
	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
	if (!m_pTextureMapVariabele->IsValid())
	{
		Logger::LogWarning(L"LoadEffectVariables error!: gTexture <-> PostBloom");
	}
}

void PostBloom::UpdateEffectVariables(RenderTarget* pRendertarget, const GameContext&)
{
	m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
}