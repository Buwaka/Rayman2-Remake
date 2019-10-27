
#include "stdafx.h"
#include "Character.h"
#include "Components.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "FollowCamera.h"
#include "ModelComponent.h"
#include "ContentManager.h"
#include "ModelAnimator.h"
#include "SoundManager.h"



Character::Character(std::wstring modelpath, UINT materialID, DirectX::XMFLOAT3 SpawnLocation, float radius, float height, float moveSpeed, float RunAcceleration, float JumpAcceleration, float ModelScale) :
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pCamera(nullptr),
	m_pController(nullptr),
	m_TotalPitch(0), 
	m_TotalYaw(0),
	m_RotationSpeed(90.f),
	//Running
	m_TerminalVelocity(moveSpeed),
	m_MaxJumpSpeed(0.3f),
	m_Gravity(9.81f), 
	m_RunAccelerationTime(1.0f), 
	m_JumpAccelerationTime(0.4f),
	m_RunAcceleration(RunAcceleration),
	m_JumpAcceleration(JumpAcceleration),
	m_RunVelocity(0), 
	m_JumpVelocity(0),
	m_Velocity(0,0,0),
	m_Timer(0),
	m_AirControlMultiplier(1.0f),
	m_ModelScale(ModelScale),
	m_CurrentState(0),
	m_SpawnLocation(SpawnLocation),
	m_SkipAbleAnimation(true),
	m_IsClimbing(false)
{
	m_pModel = new ModelComponent(modelpath);
	m_pModel->SetMaterial(materialID);
}

void Character::Initialize(const GameContext& gameContext)
{
	//TODO: Create controller
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto pDefaultMaterial = physX->createMaterial(0.5f, 0.5f, 1.0f);
	m_pController = new ControllerComponent(pDefaultMaterial,m_Radius,m_Height, L"Character", physx::PxCapsuleClimbingMode::eCONSTRAINED);
	AddComponent(m_pController);


	GameObject* model = new GameObject();
	model->AddComponent(m_pModel);
	gameContext.pScene->AddChild(model);

	//TODO: Add a fixed camera as child
	GameObject* cam = new GameObject();

	auto pCamMaterial = physX->createMaterial(0.f, 0.f, 0.1f);
	auto Controller = new ControllerComponent(pCamMaterial,0.3f,0.3f);
	cam->AddComponent(Controller);

	m_pCamera = new FollowCamera(this, Controller, DirectX::XMFLOAT3(0,0,0.2f),10.0f, 0.015f, DirectX::XMFLOAT3(0.f, 1.f, -3.5f));
	cam->AddComponent(m_pCamera);





	//std::shared_ptr<physx::PxGeometry> pSphereGeometry(new physx::PxSphereGeometry(5.f));
	//cam->AddComponent(new ColliderComponent(pSphereGeometry, *pCamMaterial));

	gameContext.pScene->AddChild(cam);

	GetTransform()->Translate(m_SpawnLocation);

	//TODO: Register all Input Actions


		//input
	auto forward = InputAction(FORWARD, InputTriggerState::Down, 'W');
	auto backward = InputAction(BACKWARD, InputTriggerState::Down, 'S');

	auto left = InputAction(LEFT, InputTriggerState::Down, 'A');
	auto right = InputAction(RIGHT, InputTriggerState::Down, 'D');

	auto jump = InputAction(UP, InputTriggerState::Pressed, VK_SPACE);
	auto hover = InputAction(HOV, InputTriggerState::Down, VK_SPACE);

	auto strafe = InputAction(STRAF, InputTriggerState::Down, VK_SHIFT);
	auto shoot = InputAction(SHOOTING, InputTriggerState::Down, VK_CONTROL);

	gameContext.pInput->AddInputAction(forward);
	gameContext.pInput->AddInputAction(backward);
	gameContext.pInput->AddInputAction(left);
	gameContext.pInput->AddInputAction(right);
	gameContext.pInput->AddInputAction(jump);
	gameContext.pInput->AddInputAction(hover);
	gameContext.pInput->AddInputAction(strafe);
	gameContext.pInput->AddInputAction(shoot);
}

void Character::PostInitialize(const GameContext&)
{
	//TODO: Set the camera as active
	// We need to do this in the PostInitialize because child game objects only get initialized after the Initialize of the current object finishes
	m_pCamera->SetActive();
	m_pModel->GetTransform()->Scale(m_ModelScale, m_ModelScale, m_ModelScale);
	m_pModel->GetAnimator()->SetAnimation(*IDLE);
	m_pModel->GetAnimator()->Play();
	m_pActor = m_pController->GetController()->getActor();

	m_pController->SetPosition(m_SpawnLocation);
	m_pModel->GetTransform()->Translate(m_SpawnLocation);
	m_pModel->GetTransform()->Rotate(0, m_TotalYaw - 180, 0);
}

void Character::Update(const GameContext& gameContext)
{
	float dTime = gameContext.pGameTime->GetElapsed();
	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&GetTransform()->GetForward());
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&GetTransform()->GetUp());
	DirectX::XMVECTOR right = DirectX::XMVector3Cross(forward, up);
	DirectX::XMVECTOR direction = forward;
	bool moved = false;

	//TODO: Update the character (Camera rotation, Character Movement, Character Gravity)

	if (gameContext.pInput->IsActionTriggered(STRAF))
	{
		if (gameContext.pInput->IsActionTriggered(LEFT))
		{
			direction = right;
			moved = true;
			ChangeState(*STRAFE, true, true);
		}
		if (gameContext.pInput->IsActionTriggered(RIGHT))
		{
			direction = DirectX::XMVectorNegate(right);
			moved = true;
			ChangeState(*STRAFE, false, true);
		}
	}
	else
	{
		if (gameContext.pInput->IsActionTriggered(LEFT))
		{
			m_TotalYaw -= m_RotationSpeed * dTime;
		}
		if (gameContext.pInput->IsActionTriggered(RIGHT))
		{
			m_TotalYaw += m_RotationSpeed * dTime;
		}
		if (gameContext.pInput->IsActionTriggered(FORWARD))
		{
			moved = true;
			ChangeState(*RUN, false, true);
		}
		if (gameContext.pInput->IsActionTriggered(BACKWARD))
		{
			direction = DirectX::XMVectorNegate(forward);
			moved = true;
			ChangeState(*RUN, true, true);
		}
	}

	//make animation skipable instead of waitfor
	//rotation
	GetTransform()->Rotate(0.0f, m_TotalYaw, 0.0f);

	//translation
	if (moved)
	{
		m_RunVelocity += (m_RunAcceleration * dTime);

		Clamp(m_RunVelocity, m_TerminalVelocity, -m_TerminalVelocity);

		auto tempy = m_Velocity.y;
		DirectX::XMStoreFloat3(&m_Velocity, DirectX::XMVectorScale(direction, m_RunVelocity));
		m_Velocity.y = tempy;

	}
	else
	{
		m_Velocity.x = 0.0f;
		m_Velocity.z = 0.0f;
		m_RunVelocity = 0;
		ChangeState(*IDLE, false,true);
	}


	//check if we are mid-air
	auto pos = m_pController->GetFootPosition();
	//offset
	pos.y = pos.y + 0.3f;
	//bool inair = true;
	physx::PxRaycastBuffer hit;
	float distance = PX_MAX_F32;
	gameContext.pScene->GetPhysxProxy()->GetPhysxScene()->raycast(ToPxVec3(pos), physx::PxVec3(0,-1,0), 2.5f, hit);

	for (size_t i = 0; i < hit.getNbAnyHits(); i++)
	{
		if (hit.getAnyHit(i).actor != m_pActor)
		{
			distance = hit.getAnyHit(i).distance;
		}
	}

	if (!m_pController->GetCollisionFlags().isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN))
	//if(inair || m_Velocity.y > 0)
	{
		if(m_Timer > gameContext.pGameTime->GetTotal())
		{
			m_Velocity.y += m_JumpAcceleration * dTime;
		}
		else if (m_IsClimbing)
		{
			m_Velocity.y = 0;
			m_Velocity.x = 0.0f;
			m_Velocity.z = 0.0f;
			ChangeState(*HANGING, false, true);
		}
		else
		{
			if (gameContext.pInput->IsActionTriggered(CharacterMovement::HOV))
			{
				m_Velocity.y -= (m_JumpAcceleration * dTime) / 8;
				ChangeState(*HOVER, false, true);
			}
			else
			{
				m_Velocity.y -= (m_JumpAcceleration * dTime);
				if (distance > 0.8f)
					ChangeState(*FALLING, false, true);
			}
		}

		m_Velocity.x *= m_AirControlMultiplier;
		m_Velocity.z *= m_AirControlMultiplier;
	}
	//if we aren't, null the vertical speed
	else
	{
		m_Velocity.y = 0.0f;
	}

	//check if we press jump
	if (gameContext.pInput->IsActionTriggered(CharacterMovement::UP) && ((m_Timer < gameContext.pGameTime->GetTotal() && distance < 0.8f) || m_IsClimbing))
	{
		m_Velocity.y = m_JumpAcceleration * dTime;
		m_Timer = gameContext.pGameTime->GetTotal() + m_JumpAccelerationTime;
		ChangeState(*JUMP,false,false,1.5f);
	}



	m_Velocity.x *= dTime;
	Clamp(m_Velocity.x, m_TerminalVelocity, -m_TerminalVelocity);

	m_Velocity.y *= dTime;
	Clamp(m_Velocity.y, m_MaxJumpSpeed, -m_MaxJumpSpeed);

	m_Velocity.z *= dTime;
	Clamp(m_Velocity.z, m_TerminalVelocity, -m_TerminalVelocity);

	m_pController->Move(m_Velocity);
	m_pModel->GetTransform()->Translate(m_pController->GetFootPosition());
	m_pModel->GetTransform()->Rotate(0, m_TotalYaw - 180,0);
}

void Character::SetAnimationIDs(UINT run, UINT idle, UINT jump, UINT shoot, UINT strafe, UINT hover, UINT hanging, UINT falling, UINT fastrun)
{
	m_AnimationIDs[0] = run;
	m_AnimationIDs[1] = idle;
	m_AnimationIDs[2] = jump;
	m_AnimationIDs[3] = shoot;
	m_AnimationIDs[4] = strafe;
	m_AnimationIDs[5] = hover;
	m_AnimationIDs[6] = hanging;
	m_AnimationIDs[7] = falling;
	m_AnimationIDs[8] = fastrun;
}

void Character::ChangeState(UINT state, bool reverse, bool Skipable, float speed)
{
	if (state == m_CurrentState)
		return;

	if (!m_SkipAbleAnimation )
	{
		if (!m_pModel->GetAnimator()->IsFinished())
			return;
	}

	m_pModel->GetAnimator()->SetAnimation(state, speed);
	m_pModel->GetAnimator()->SetPlayReversed(reverse);
	m_CurrentState = state;
	m_SkipAbleAnimation = Skipable;

	if(!Skipable)
		m_pModel->GetAnimator()->WaitForFinish();

	//if (WaitforCurrentAnimation)
	//{
	//	m_pModel->GetAnimator()->WaitForFinish();

	//	if (m_pModel->GetAnimator()->IsFinished())
	//	{
	//		m_pModel->GetAnimator()->SetAnimation(state);
	//		m_pModel->GetAnimator()->SetPlayReversed(reverse);
	//		m_CurrentState = state;
	//	}

	//}
	//else
	//{
	//	m_pModel->GetAnimator()->SetAnimation(state);
	//	m_pModel->GetAnimator()->SetPlayReversed(reverse);
	//	m_CurrentState = state;
	//}


}

void Character::SetPosition(DirectX::XMFLOAT3 position)
{
	m_pController->SetPosition(position);
	m_pModel->GetTransform()->Translate(position);
	m_TotalYaw = 0;
	m_pModel->GetTransform()->Rotate(0, m_TotalYaw - 180, 0);
}
