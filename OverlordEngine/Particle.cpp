#include "stdafx.h"
#include "Particle.h"
#include "MathHelper.h"



Particle::Particle(const ParticleEmitterSettings& emitterSettings):
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_IsActive(false),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0)
{}

void Particle::Update(const GameContext& gameContext)
{
	using namespace DirectX;
	if (!m_IsActive)
	{
		return;
	}

	m_CurrentEnergy -= gameContext.pGameTime->GetElapsed();
	if (m_CurrentEnergy <= 0.0f)
	{
		m_IsActive = false;
		return;
	}

	XMVECTOR vertexPos = DirectX::XMLoadFloat3(&m_VertexInfo.Position);
	XMVECTOR emitVel = DirectX::XMLoadFloat3(&m_EmitterSettings.Velocity);
	XMVECTOR calculateVal = vertexPos + emitVel * gameContext.pGameTime->GetElapsed();

	DirectX::XMStoreFloat3(&m_VertexInfo.Position, calculateVal);
	m_VertexInfo.Color = m_EmitterSettings.Color;
	
	float particleLifePercent = m_CurrentEnergy / m_TotalEnergy;
	m_VertexInfo.Color.w = particleLifePercent * 2;

	if (m_SizeGrow < 1.0f)
	{
		m_VertexInfo.Size = m_InitSize - (1.0f - particleLifePercent) * m_SizeGrow;
	}
	else if (m_SizeGrow > 1.0f)
	{
		m_VertexInfo.Size = m_InitSize + (1.0f - particleLifePercent) * m_SizeGrow;
	}
}

void Particle::Init(DirectX::XMFLOAT3 initPosition, bool randomRotation, float rotation)
{
	using namespace DirectX;
	m_IsActive = true;

	m_TotalEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	m_CurrentEnergy = m_TotalEnergy;

	DirectX::XMVECTOR unitVector = { 1,0,0 };
	DirectX::XMMATRIX randRot = DirectX::XMMatrixRotationRollPitchYaw(randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI));
	DirectX::XMVECTOR randRotVec = DirectX::XMVector3TransformNormal(unitVector, randRot);

	float distance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);




	DirectX::XMVECTOR initPos = DirectX::XMLoadFloat3(&initPosition);
	DirectX::XMVECTOR calculateVal = initPos + randRotVec * distance;
	DirectX::XMStoreFloat3(&m_VertexInfo.Position, calculateVal);


	m_VertexInfo.Size = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);
	m_InitSize = m_VertexInfo.Size;

	if(randomRotation)
		m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);
	else
		m_VertexInfo.Rotation = rotation;
}
