#include "stdafx.h"
#include "StaticParticle.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "TransformComponent.h"
#include "EffectHelper.h"
#include "ParticleEmitterComponent.h"

StaticParticle::StaticParticle(std::wstring assetFile, DirectX::XMFLOAT3 Position, DirectX::XMFLOAT4 Color, float size, float rotation) :
	m_pVertexBuffer(nullptr),
	m_pEffect(nullptr),
	m_pParticleTexture(nullptr),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_AssetFile(std::move(assetFile)),
	m_Changed(true),
	m_Active(true)
{
	m_ParticleVertex = new ParticleVertex(Position, Color,size, rotation);
}


StaticParticle::~StaticParticle()
{
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pInputLayout);
	if(m_ParticleVertex)
		delete m_ParticleVertex;
}

void StaticParticle::SetRotation(float degrees)
{
	m_ParticleVertex->Rotation = degrees;
	m_Changed = true;
}

void StaticParticle::LoadEffect(const GameContext & gameContext)
{
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ParticleRenderer.fx");
	m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);

	if (!m_pWvpVariable)
	{
		m_pWvpVariable = m_pEffect->GetVariableBySemantic("WorldViewProjection")->AsMatrix();
	}

	if (!m_pViewInverseVariable)
	{
		m_pViewInverseVariable = m_pEffect->GetVariableBySemantic("ViewInverse")->AsMatrix();
	}

	if (!m_pParticleTexture)
	{
		m_pTextureVariable = m_pEffect->GetVariableByName("gParticleTexture")->AsShaderResource();
	}


	std::vector<ILDescription> inputLayoutDesc{};
	UINT inputLayoutID{};
	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pDefaultTechnique, &m_pInputLayout, inputLayoutDesc, m_pInputLayoutSize, inputLayoutID);
}

void StaticParticle::CreateVertexBuffer(const GameContext & gameContext)
{
	SafeRelease(m_pVertexBuffer);

	D3D11_BUFFER_DESC buffer = {};
	buffer.ByteWidth = sizeof(ParticleVertex);
	buffer.MiscFlags = 0;
	buffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer.Usage = D3D11_USAGE_DYNAMIC;
	buffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	Logger::LogHResult(gameContext.pDevice->CreateBuffer(&buffer, nullptr, &m_pVertexBuffer), L"Failed to create buffer!");
}

void StaticParticle::Initialize(const GameContext & gameContext)
{
	LoadEffect(gameContext);
	CreateVertexBuffer(gameContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void StaticParticle::Update(const GameContext & gameContext)
{
	if (m_Changed && m_Active)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		ParticleVertex* pBuffer = (ParticleVertex*)mappedResource.pData;

		pBuffer[0] = *m_ParticleVertex;

		gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
		m_Changed = false;
	}

}

void StaticParticle::PostDraw(const GameContext & gameContext)
{
	if (!m_Active)
		return;

	DirectX::XMMATRIX viewProj = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
	m_pWvpVariable->SetMatrix((float*)(&viewProj));

	DirectX::XMMATRIX viewInverse = XMLoadFloat4x4(&gameContext.pCamera->GetViewInverse());
	m_pViewInverseVariable->SetMatrix((float*)(&viewInverse));

	m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());

	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	UINT offset = 0;
	UINT stride = sizeof(ParticleVertex);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC desc{};
	m_pDefaultTechnique->GetDesc(&desc);

	for (UINT i = 0; i < desc.Passes; ++i)
	{
		m_pDefaultTechnique->GetPassByIndex(i)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(3, 0);
	}
}
