//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "DiffuseMaterial_Shadow_Panning.h"
#include "GeneralStructs.h"
#include "Logger.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "Components.h"

ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow_Panning::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectShaderResourceVariable* DiffuseMaterial_Shadow_Panning::m_pShadowSRVvariable = nullptr;
ID3DX11EffectVectorVariable* DiffuseMaterial_Shadow_Panning::m_pLightDirectionVariable = nullptr;
ID3DX11EffectMatrixVariable* DiffuseMaterial_Shadow_Panning::m_pLightWVPvariable = nullptr;
ID3DX11EffectVectorVariable* DiffuseMaterial_Shadow_Panning::m_pPanningVariabele = nullptr;

DiffuseMaterial_Shadow_Panning::DiffuseMaterial_Shadow_Panning() : Material(L"./Resources/Effects/Shadow/PosNormTex3D_Shadow_Panning.fx"),
	m_pDiffuseTexture(nullptr),
	m_Direction(DirectX::XMFLOAT2(1,0)),
	m_Speed(1.0f)
{}

void DiffuseMaterial_Shadow_Panning::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void DiffuseMaterial_Shadow_Panning::SetDiffuseTexture(TextureData * texture)
{
	m_pDiffuseTexture = texture;
}

void DiffuseMaterial_Shadow_Panning::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	m_LightDirection = dir;
}

void DiffuseMaterial_Shadow_Panning::LoadEffectVariables()
{
	//TODO: load all the necessary shader variables
	m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();

	m_pShadowSRVvariable = GetEffect()->GetVariableByName("gShadowMap")->AsShaderResource();

	m_pLightDirectionVariable = GetEffect()->GetVariableByName("gLightDirection")->AsVector();

	m_pLightWVPvariable = GetEffect()->GetVariableByName("gWorldViewProj_Light")->AsMatrix();

	m_pPanningVariabele = GetEffect()->GetVariableByName("gPanning")->AsVector();

}

void DiffuseMaterial_Shadow_Panning::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	if (m_pDiffuseTexture == nullptr)
		return;

	using namespace DirectX;

	auto dirvector = DirectX::XMLoadFloat2(&m_Direction);
	dirvector = dirvector * (gameContext.pGameTime->GetTotal() * m_Speed);
	DirectX::XMFLOAT2 panning;
	DirectX::XMStoreFloat2(&panning, dirvector);

	m_pPanningVariabele->SetFloatVector(reinterpret_cast<float*>(&panning));

	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

	m_pShadowSRVvariable->SetResource(gameContext.pShadowMapper->GetShadowMap());

	m_pLightDirectionVariable->SetFloatVector(reinterpret_cast<float*>(&m_LightDirection));

	
	
	auto world = XMLoadFloat4x4(&pModelComponent->GetTransform()->GetWorld());
	auto lvp = gameContext.pShadowMapper->GetLightVP();
	auto viewProjection = XMLoadFloat4x4(&lvp);

	auto wvp = world * viewProjection;
	m_pLightWVPvariable->SetMatrix(reinterpret_cast<float*>(&wvp));


	//TODO: update all the necessary shader variables
}