#pragma once
#include "GameObject.h"
#include "ParticleEmitterComponent.h"
#include "StaticParticle.h"

class ColliderComponent;

class Lum :	public GameObject
{
public:
	Lum(DirectX::XMFLOAT3 position);
	~Lum();

	void SetCallback(GameObject::PhysicsCallback callback) { m_pCallback = callback; };

	void SetActive(bool active);
	bool IsActive() { return m_Active; };

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

private:
	void MakeParticleObject();



	bool m_Active;
	GameObject::PhysicsCallback m_pCallback;
	ColliderComponent* m_pTrigger;
	ParticleEmitterComponent* m_pParticleEmitter;
	StaticParticle* m_pStaticParticle;
	DirectX::XMFLOAT3 m_position;
};

