#pragma once
#include "GameObject.h"

class ColliderComponent;
class StaticParticle;
class EndPortal : public GameObject
{
public:
	EndPortal(DirectX::XMFLOAT3 position, float radius, GameObject* Actor, float rotationspeed = 1.f);
	~EndPortal() = default;

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext&) override;

private:
	GameObject* m_pActor;
	ColliderComponent* m_pTrigger;
	StaticParticle* m_pStaticParticle;
	DirectX::XMFLOAT3 m_Position;
	float m_Radius;
	float m_counter;
	float m_RotationSpeed;
};

