#include "stdafx.h"
#include "EndPortal.h"
#include "SceneManager.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "PhysxManager.h"
#include "ColliderComponent.h"
#include "StaticParticle.h"
#include "ParticleEmitterComponent.h"
#include "RaymanGame.h"


EndPortal::EndPortal(DirectX::XMFLOAT3 position, float radius, GameObject* Actor, float rotationspeed) :
	m_pTrigger(nullptr),
	m_Radius(radius),
	m_Position(position),
	m_pActor(Actor),
	m_counter(0.f),
	m_RotationSpeed(rotationspeed)
{

}

void EndPortal::Initialize(const GameContext &)
{

	auto pRigidbody = new RigidBodyComponent();
	pRigidbody->SetActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);

	AddComponent(pRigidbody);

	auto pPhysx = PhysxManager::GetInstance()->GetPhysics();
	auto pDefaultMaterial = pPhysx->createMaterial(0, 0, 0);
	std::shared_ptr<physx::PxGeometry> pSphereGeometry(new physx::PxBoxGeometry(5,1,5));
	m_pTrigger = new ColliderComponent(pSphereGeometry, *pDefaultMaterial);
	m_pTrigger->EnableTrigger(true);
	AddComponent(m_pTrigger);


	auto Climbing = [actor = m_pActor](GameObject*, GameObject* otherobject, GameObject::TriggerAction )
	{
		if (otherobject == actor)
		{
			SceneManager::GetInstance()->GetGameScene(L"RaymanGame")->StopMusic();
			SceneManager::GetInstance()->SetActiveGameScene(L"EndingMenu");
			SceneManager::GetInstance()->GetGameScene(L"EndingMenu")->StartMusic();

		}

	};

	SetOnTriggerCallBack(Climbing);


	m_pStaticParticle = new StaticParticle(L"./Resources/Textures/portal.png", m_Position, DirectX::XMFLOAT4(DirectX::Colors::White),20.0f);
	AddComponent(m_pStaticParticle);

	auto m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/particles/glowingdots.png", 5);
	m_pParticleEmitter->SetVelocity(DirectX::XMFLOAT3(0.f, -0.5f, 0.f));
	m_pParticleEmitter->SetMinSize(0.1f);
	m_pParticleEmitter->SetMaxSize(1.5f);
	m_pParticleEmitter->SetMinEnergy(1.0f);
	m_pParticleEmitter->SetMaxEnergy(2.0f);
	m_pParticleEmitter->SetMinSizeGrow(0.3f);
	m_pParticleEmitter->SetMaxSizeGrow(1.8f);
	m_pParticleEmitter->SetMinEmitterRange(0.1f);
	m_pParticleEmitter->SetMaxEmitterRange(1.3f);
	m_pParticleEmitter->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.0f));
	AddComponent(m_pParticleEmitter);


	GetTransform()->Translate(m_Position);
}

void EndPortal::Update(const GameContext & gameContext)
{
	m_counter += gameContext.pGameTime->GetElapsed() * m_RotationSpeed;
	m_counter = std::fmodf(m_counter, DirectX::XM_PI);

	m_pStaticParticle->SetRotation(m_counter);
}