#include "stdafx.h"
#include "Lum.h"
#include "ParticleEmitterComponent.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "PhysxManager.h"
#include "SceneManager.h"
#include "GameScene.h"



Lum::Lum(DirectX::XMFLOAT3 position) :
	m_pParticleEmitter(nullptr),
	m_pTrigger(nullptr),
	m_position(position),
	m_pCallback(nullptr),
	m_Active(true)
{
}


Lum::~Lum()
{
}

void Lum::SetActive(bool active)
{
	if (active == m_Active)
		return;

	if (active)
	{
		m_pParticleEmitter->SetActive(true);
		m_pStaticParticle->SetActive(true);
	}
	else
	{
		m_pParticleEmitter->SetActive(false);
		m_pStaticParticle->SetActive(false);
	}

	m_Active = active;
}

void Lum::Initialize(const GameContext &)
{
	MakeParticleObject();


	auto pRigidbody = new RigidBodyComponent();
	pRigidbody->SetActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);

	AddComponent(pRigidbody);

	auto pPhysx = PhysxManager::GetInstance()->GetPhysics();
	auto pDefaultMaterial = pPhysx->createMaterial(0, 0, 0);
	std::shared_ptr<physx::PxGeometry> pSphereGeometry(new physx::PxSphereGeometry(0.3f));
	m_pTrigger = new ColliderComponent(pSphereGeometry, *pDefaultMaterial);
	m_pTrigger->EnableTrigger(true);
	AddComponent(m_pTrigger);

	SetOnTriggerCallBack(m_pCallback);

	GetTransform()->Translate(m_position);

}

void Lum::Update(const GameContext &)
{
}

void Lum::MakeParticleObject()
{
	m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/particles/glowingdots.png", 5);
	m_pParticleEmitter->SetVelocity(DirectX::XMFLOAT3(0.f, -0.5f, 0.f));
	m_pParticleEmitter->SetMinSize(0.1f);
	m_pParticleEmitter->SetMaxSize(0.5f);
	m_pParticleEmitter->SetMinEnergy(1.0f);
	m_pParticleEmitter->SetMaxEnergy(2.0f);
	m_pParticleEmitter->SetMinSizeGrow(0.3f);
	m_pParticleEmitter->SetMaxSizeGrow(0.8f);
	m_pParticleEmitter->SetMinEmitterRange(0.1f);
	m_pParticleEmitter->SetMaxEmitterRange(0.3f);
	m_pParticleEmitter->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.0f));
	AddComponent(m_pParticleEmitter);


	m_pStaticParticle = new StaticParticle(L"./Resources/Textures/lum.png", m_position);
	AddComponent(m_pStaticParticle);
}
