#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(std::vector<DirectX::XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_Transition(false),
m_TickCount(0),
m_AnimationSpeed(1.0f),
m_TransitionLength(0.1f),
m_TransitionTimer(0.f),
m_WaitForFinish(false),
m_IsFinished(false)
{
	SetAnimation(0);
	m_LastTransform = m_Transforms;
	m_LastKey = m_CurrentClip.Keys.front();

#ifdef _DEBUG
	//just set the first bone to identity for debug sake
	//m_Transforms.resize(1);
	//DirectX::XMStoreFloat4x4(&m_Transforms[0], DirectX::XMMatrixIdentity());
#endif // DEBUG
}

void ModelAnimator::SetAnimation(UINT clipNumber, float speed)
{
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	auto Anims = m_pMeshFilter->m_AnimationClips;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (Anims.size() < clipNumber)
	{
		Reset();
		Logger::LogError(L"ModelAnimator::SetAnimation provided clipnumber is not in range");
	}
	else
	{
		SetAnimation(Anims[clipNumber], speed);
	}
	//If not,
		//	Call Reset
		//	Log a warning with an appropriate message
		//	return
	//else
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)
}

void ModelAnimator::SetAnimation(std::wstring clipName, float speed)
{
	//TODO: complete
	//Set m_ClipSet to false
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	//If found,
	//	Call SetAnimation(Animation Clip) with the found clip
	//Else
	//	Call Reset
	//	Log a warning with an appropriate message

	m_ClipSet = false;
	auto Anims = m_pMeshFilter->m_AnimationClips;
	auto found = std::find_if(Anims.begin(), Anims.end(), [&clipName](const AnimationClip& A) {return A.Name == clipName; });

	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (found == Anims.end())
	{
		Reset();
		Logger::LogError(L"ModelAnimator::SetAnimation provided clipnumber is not in range");
	}
	else
	{
		SetAnimation(*found, speed);
	}
}

void ModelAnimator::SetAnimation(AnimationClip clip, float speed)
{
	//TODO: complete
	//Set m_ClipSet to true

	m_ClipSet = true;
	m_Transition = true;
	m_LastTransform = m_Transforms;
	m_TransitionTimer = m_TransitionLength * clip.TicksPerSecond;
	//Set m_CurrentClip
	m_CurrentClip = clip;
	//Call Reset(false)
	Reset(false);

	m_AnimationSpeed = speed;
}

void ModelAnimator::Reset(bool pause)
{
	//TODO: complete
	//If pause is true, set m_IsPlaying to false
	if (pause)
		m_IsPlaying = false;

	//Set m_TickCount to zero
	m_TickCount = 0;
	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.0f;

	if (m_ClipSet)
	{
		auto &trans = m_CurrentClip.Keys[0].BoneTransforms;
		m_Transforms = trans;
	}
	else
	{
		DirectX::XMFLOAT4X4 unit;
		DirectX::XMStoreFloat4x4(&unit, DirectX::XMMatrixIdentity());
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, unit);
	}
	
	
	//If m_ClipSet is true
	//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
	//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
	//Else
	//	Create an IdentityMatrix 
	//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	//TODO: complete
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		m_IsFinished = false;
		auto passedTicks = gameContext.pGameTime->GetElapsed() * m_CurrentClip.TicksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)

		//2. 
		if (m_Transition)
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.Duration)
			{
				m_TickCount -= m_CurrentClip.Duration;
				m_TickCount = std::fmodf(m_TickCount, m_CurrentClip.Duration);
				m_Transition = false;
			}
		}
		else
		{
			if (m_Reversed)
			{
				m_TickCount -= passedTicks;
				if (m_TickCount < 0)
					m_TickCount += m_CurrentClip.Duration;
			}
			else
			{
				m_TickCount += passedTicks;
				if (m_TickCount > m_CurrentClip.Duration)
				{
					m_TickCount -= m_CurrentClip.Duration;
					m_TickCount = std::fmodf(m_TickCount, m_CurrentClip.Duration);
					if (m_WaitForFinish)
					{
						m_WaitForFinish = false;
						m_IsFinished = true;
					}

				}

			}
		}

		//IF m_Reversed is true
		//	Subtract passedTicks from m_TickCount
		//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
		//ELSE
		//	Add passedTicks to m_TickCount
		//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount

		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		float BlendFactor;

		if (m_Transition && m_TickCount < m_TransitionTimer)
		{
			keyA = m_LastKey;
			keyB = m_CurrentClip.Keys.front();

			BlendFactor = (m_TickCount / m_TransitionTimer);
		}
		else
		{
			m_Transition = false;
			//Iterate all the keys of the clip and find the following keys:

			//keyA > Closest Key with Tick before/smaller than m_TickCount
			//keyB > Closest Key with Tick after/bigger than m_TickCount
			auto keys = m_CurrentClip.Keys;
			auto JustAbove = std::lower_bound(keys.begin(), keys.end(), m_TickCount);

			if (JustAbove == keys.end())
				keyB = keys.back();
			else
				keyB = *JustAbove;

			if (JustAbove == keys.begin())
				keyA = keys.back();
			else
				keyA = *(--JustAbove);

			BlendFactor = 1.f - ((keyB.Tick - m_TickCount) / (keyB.Tick - keyA.Tick));
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)

		//Clear the m_Transforms vector
		//m_Transforms.clear();

		//uncomment if bonecount changes
		if(m_Transforms.size() < keyA.BoneTransforms.size())
			m_Transforms.resize(keyA.BoneTransforms.size());


		//FOR every boneTransform in a key (So for every bone)
		for (size_t i = 0; i < keyA.BoneTransforms.size(); i++)
		{
			DirectX::XMFLOAT4X4 transformA;

			if (m_Transition)
				transformA = m_LastTransform[i];
			else
				transformA = keyA.BoneTransforms[i];

			DirectX::XMMATRIX matA = DirectX::XMLoadFloat4x4(&transformA);
			DirectX::XMVECTOR posA;
			DirectX::XMVECTOR rotA;
			DirectX::XMVECTOR sclA;
			DirectX::XMMatrixDecompose(&sclA, &rotA, &posA, matA);


			auto& transformB = keyB.BoneTransforms[i];
			DirectX::XMMATRIX matB = DirectX::XMLoadFloat4x4(&transformB);
			DirectX::XMVECTOR posB;
			DirectX::XMVECTOR rotB;
			DirectX::XMVECTOR sclB;
			DirectX::XMMatrixDecompose(&sclB, &rotB, &posB, matB);




			DirectX::XMVECTOR posC = DirectX::XMVectorLerp(posA, posB, BlendFactor);
			DirectX::XMVECTOR rotC = DirectX::XMQuaternionSlerp(rotA, rotB, BlendFactor);
			DirectX::XMVECTOR sclC = DirectX::XMVectorLerp(sclA, sclB, BlendFactor);
			DirectX::XMMATRIX matC = DirectX::XMMatrixAffineTransformation(sclC, DirectX::XMVECTOR{0}, rotC, posC);
			DirectX::XMStoreFloat4x4(&m_Transforms[i], matC);
		}

		m_LastKey = keyA;
		//	Retrieve the transform from keyA (transformA)
		//	auto transformA = ...
		// 	Retrieve the transform from keyB (transformB)
		//	auto transformB = ...
		//	Decompose both transforms
		//	Lerp between all the transformations (Position, Scale, Rotation)
		//	Compose a transformation matrix with the lerp-results
		//	Add the resulting matrix to the m_Transforms vector
	}
}
