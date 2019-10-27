#pragma once
#include <CameraComponent.h>
#include "ControllerComponent.h"
class FollowCamera : public CameraComponent
{
public:
	FollowCamera(GameObject* Target, ControllerComponent* Controller, DirectX::XMFLOAT3 distance, float Maxdistance, float maxSpeed = 1.0f, DirectX::XMFLOAT3 LookAtOffset = DirectX::XMFLOAT3(0.f,0.f,0.f));
	~FollowCamera() = default;

	void SetTarget(GameObject* Target) { m_Target = Target; };
	GameObject* GetTarget() { return m_Target; };

protected:
	void Update(const GameContext& gameContext) override;
	void Initialize(const GameContext& gameContext) override;

private:
	GameObject* m_Target;
	ControllerComponent* m_Controller;
	DirectX::XMFLOAT3 m_Distance;
	DirectX::XMFLOAT3 m_LookAtOffset;
	float m_MaxSpeed;
	float m_MaxDistance;
};

