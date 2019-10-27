#include "stdafx.h"
#include "ClimbArea.h"
#include "PhysxManager.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "Character.h"


ClimbArea::ClimbArea(DirectX::XMFLOAT3 position, float height, float width, Character * chara) :
	m_pTrigger(nullptr),
	m_Position(position),
	m_Width(width),
	m_Height(height),
	m_pCharacter(chara)
{
}

ClimbArea::~ClimbArea()
{
}

void ClimbArea::Initialize(const GameContext &)
{
	auto pRigidbody = new RigidBodyComponent();
	pRigidbody->SetActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);

	AddComponent(pRigidbody);

	auto pPhysx = PhysxManager::GetInstance()->GetPhysics();
	auto pDefaultMaterial = pPhysx->createMaterial(0, 0, 0);
	std::shared_ptr<physx::PxGeometry> pSphereGeometry(new physx::PxBoxGeometry(m_Width,m_Height,m_Width));
	m_pTrigger = new ColliderComponent(pSphereGeometry, *pDefaultMaterial);
	m_pTrigger->EnableTrigger(true);
	AddComponent(m_pTrigger);


	auto Climbing = [actor = (Character*)m_pCharacter](GameObject*, GameObject* otherobject, GameObject::TriggerAction action)
	{
		if (otherobject == (GameObject*)actor)
		{
			switch (action)
			{
			case GameObject::TriggerAction::ENTER:
				actor->SetClimbing(true);
				auto rot = otherobject->GetTransform()->GetWorldRotation();
				actor->SetRotation(rot.x);
				break;
			case GameObject::TriggerAction::LEAVE:
				actor->SetClimbing(false);
				break;
			default:
				break;
			}
		}
	};


	SetOnTriggerCallBack(Climbing);

	GetTransform()->Translate(m_Position);
}
