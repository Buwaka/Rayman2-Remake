#include "stdafx.h"
#include "FollowCamera.h"
#include "TransformComponent.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "GameScene.h"
#include "RigidBodyComponent.h"
#include "CameraComponent.h"
#include "OverlordGame.h"
#include "TransformComponent.h"
#include "ControllerComponent.h"
#include "GeneralStructs.h"


FollowCamera::FollowCamera(GameObject* Target, ControllerComponent* Controller, DirectX::XMFLOAT3 Distance, float Maxdistance,float maxSpeed, DirectX::XMFLOAT3 LookAtOffset) :
	m_Target(Target),
	m_Distance(Distance),
	m_Controller(Controller),
	m_LookAtOffset(LookAtOffset),
	m_MaxSpeed(maxSpeed),
	m_MaxDistance(Maxdistance)
{
}

void FollowCamera::Initialize(const GameContext&)
{
	m_Controller->GetController()->getActor()->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	//Controller->SetConstraint(RigidBodyConstraintFlag::RotY, false);
	//Controller->SetConstraint(RigidBodyConstraintFlag::RotZ, false);
	//Controller->SetActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	const DirectX::XMVECTOR TargetPosition = XMLoadFloat3(&m_Target->GetTransform()->GetWorldPosition());
	DirectX::XMVECTOR worldPosition = DirectX::XMVectorAdd(TargetPosition, XMLoadFloat3(&m_Distance));
	GetTransform()->Translate(worldPosition);

	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, worldPosition);
	m_Controller->SetPosition(temp);
	GetTransform()->Translate(temp);
}

void FollowCamera::Update(const GameContext & context)
{
	const auto distance = XMLoadFloat3(&m_Distance);
	const auto distanceLength = DirectX::XMVector3Length(distance);


	const DirectX::XMVECTOR TargetPosition = XMLoadFloat3(&m_Target->GetTransform()->GetWorldPosition());

	auto rot = m_Target->GetTransform()->GetRotation();
	DirectX::XMVECTOR rotatedoffset = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&m_LookAtOffset), DirectX::XMLoadFloat4(&rot));
	const DirectX::XMVECTOR TargetPositionWithOffset = DirectX::XMVectorAdd(TargetPosition, rotatedoffset);

	auto contpos = m_Controller->GetController()->getPosition();
	auto contposdx = DirectX::XMFLOAT3((float)contpos.x, (float)contpos.y, (float)contpos.z);
	DirectX::XMVECTOR CurrentPosition = DirectX::XMLoadFloat3(&contposdx);

	auto Displacement = DirectX::XMVectorSubtract(TargetPositionWithOffset, CurrentPosition);
	auto DisplacementLength = DirectX::XMVector3Length(Displacement);

	auto newPosition = CurrentPosition;

	if (DirectX::XMVector3Greater(DisplacementLength, distanceLength))
	{
		auto maxdistxm = DirectX::XMFLOAT3(m_MaxDistance, m_MaxDistance, m_MaxDistance);
		auto maxdistance = XMLoadFloat3(&maxdistxm);

		if (DirectX::XMVector3GreaterOrEqual(maxdistance, DisplacementLength))
		{
			//displength / maxdistance * disp / displength / speed
			//	x / y * z / x / w
			// = z / yw
			auto speed1 = DirectX::XMFLOAT3(m_MaxSpeed, m_MaxSpeed, m_MaxSpeed);
			auto speed = DirectX::XMLoadFloat3(&speed1);
			auto speedfactor = DirectX::XMVectorMultiply(maxdistance, speed);
			auto dtime1 = DirectX::XMFLOAT3(context.pGameTime->GetElapsed(), context.pGameTime->GetElapsed(), context.pGameTime->GetElapsed());
			auto dtime = DirectX::XMLoadFloat3(&dtime1);


			DirectX::XMFLOAT3 normalizedDisplacement;
			DirectX::XMStoreFloat3(&normalizedDisplacement, DirectX::XMVectorMultiply(dtime, DirectX::XMVectorDivide(Displacement, speedfactor)));
			m_Controller->Move(normalizedDisplacement);

			auto temp = m_Controller->GetController()->getPosition();
			auto temp2 = DirectX::XMFLOAT3((float)temp.x, (float)temp.y, (float)temp.z);
			newPosition = DirectX::XMLoadFloat3(&temp2);
		}
		else
		{
			DirectX::XMFLOAT3 temp;
			DirectX::XMStoreFloat3(&temp, TargetPositionWithOffset);
			m_Controller->SetPosition(temp);

			newPosition = TargetPositionWithOffset;
		}

	}


	using namespace DirectX;

	const auto windowSettings = OverlordGame::GetGameSettings().Window;
	DirectX::XMMATRIX projection;

	if (m_PerspectiveProjection)
	{
		projection = DirectX::XMMatrixPerspectiveFovLH(m_FOV, windowSettings.AspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
		const float viewHeight = (m_Size > 0) ? m_Size : windowSettings.Height;
		projection = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}


	const DirectX::XMVECTOR lookAt = XMVectorSubtract(TargetPosition, newPosition);
	//const DirectX::XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const DirectX::XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(newPosition, newPosition + lookAt, upVec);
	const DirectX::XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const DirectX::XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}
