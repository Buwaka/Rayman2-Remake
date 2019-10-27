#pragma once
#include "GameObject.h"

class ControllerComponent;
class CameraComponent;
class ModelComponent;
class SoundManager;

const int AnimationCount = 9;

class Character : public GameObject
{
public:
	enum CharacterMovement : UINT
	{
		LEFT = 0,
		RIGHT,
		FORWARD,
		BACKWARD,
		UP,
		HOV,
		STRAF,
		SHOOTING
	};

	Character(std::wstring modelpath, UINT materialID, DirectX::XMFLOAT3 SpawnLocation, float radius = 2, float height = 5, float moveSpeed = 100, float RunAcceleration = 0.5f, float JumpAcceleration = 0.5f, float ModelScale = 1.0f);
	virtual ~Character() = default;

	Character(const Character& other) = delete;
	Character(Character&& other) noexcept = delete;
	Character& operator=(const Character& other) = delete;
	Character& operator=(Character&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

	void SetAnimationIDs(UINT run, UINT idle, UINT jump, UINT shoot, UINT strafe, UINT hover, UINT hanging, UINT falling, UINT fastrun);
	void ChangeState(UINT state, bool reverse = false, bool Skipable = false, float speed = 1.0f);
	void SetPosition(DirectX::XMFLOAT3 position);
	bool IsClimbing() { return m_IsClimbing; };
	void SetClimbing(bool value) { m_IsClimbing = value; };
	void SetRotation(float rot) { m_TotalYaw = rot - 180; }


	CameraComponent* GetCamera() const { return m_pCamera; }
	physx::PxActor* GetActor() const { return m_pActor; }

protected:

	CameraComponent* m_pCamera;
	ControllerComponent* m_pController;
	ModelComponent* m_pModel;
	physx::PxActor* m_pActor;

	float m_TotalPitch, m_TotalYaw;
	float m_MoveSpeed, m_RotationSpeed;
	float m_Radius, m_Height;
	bool m_SkipAbleAnimation;
	bool m_IsClimbing;
	UINT m_CurrentState;
	DirectX::XMFLOAT3 m_SpawnLocation;

	//Running
	float m_TerminalVelocity, 
		m_MaxJumpSpeed,
		m_Gravity, 
		m_RunAccelerationTime, 
		m_JumpAccelerationTime, 
		m_RunAcceleration, 
		m_JumpAcceleration, 
		m_RunVelocity, 
		m_JumpVelocity,m_Timer, m_AirControlMultiplier, m_ModelScale;

	DirectX::XMFLOAT3 m_Velocity;


	enum AnimationID : UINT
	{
		_RUN = 0,
		_IDLE = 1,
		_JUMP = 2,
		_SHOOT = 3,
		_STRAFE = 4,
		_HOVER = 5,
		_HANGING = 6,
		_FALLING = 7,
		_FASTRUN = 8
	};

	//RUN = 0,IDLE = 1,JUMP = 2,SHOOT = 3,STRAFE = 4,HOVER = 5,HANGING = 6,FALLING = 7,FASTRUN = 8
	UINT m_AnimationIDs[9] = { _RUN, _IDLE, _JUMP, _SHOOT, _STRAFE, _HOVER, _HANGING, _FALLING, _FASTRUN };
	const UINT* RUN =		&m_AnimationIDs[_RUN];
	const UINT* IDLE =		&m_AnimationIDs[_IDLE];
	const UINT* JUMP =		&m_AnimationIDs[_JUMP];
	const UINT* SHOOT =		&m_AnimationIDs[_SHOOT];
	const UINT* STRAFE =	&m_AnimationIDs[_STRAFE];
	const UINT* HOVER =		&m_AnimationIDs[_HOVER];
	const UINT* HANGING =	&m_AnimationIDs[_HANGING];
	const UINT* FALLING =	&m_AnimationIDs[_FALLING];
	const UINT* FASTRUN =	&m_AnimationIDs[_FASTRUN];
};


