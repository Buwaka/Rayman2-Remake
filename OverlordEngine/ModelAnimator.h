#pragma once
#include "MeshFilter.h"

class ModelAnimator final
{
public:
	ModelAnimator(MeshFilter* pMeshFilter);
	~ModelAnimator() = default;

	ModelAnimator(const ModelAnimator& other) = delete;
	ModelAnimator(ModelAnimator&& other) noexcept = delete;
	ModelAnimator& operator=(const ModelAnimator& other) = delete;
	ModelAnimator& operator=(ModelAnimator&& other) noexcept = delete;
	
	void SetAnimation(std::wstring clipName, float speed = 1.0f);
	void SetAnimation(UINT clipNumber, float speed = 1.0f);
	void SetAnimation(AnimationClip clip, float speed = 1.0f);
	void Update(const GameContext& gameContext);
	void Reset(bool pause = true);
	void Play() { m_IsPlaying = true; }
	void Pause() { m_IsPlaying = false; }
	void SetPlayReversed(bool reverse) { m_Reversed = reverse; }
	void SetAnimationSpeed(float speedPercentage) { m_AnimationSpeed = speedPercentage; }

	void WaitForFinish() { m_WaitForFinish = true; };
	bool IsFinished() { return m_IsFinished; };

	bool IsPlaying() const { return m_IsPlaying; }
	bool IsReversed() const { return m_Reversed; }
	float GetAnimationSpeed() const { return m_AnimationSpeed; }
	UINT GetClipCount() const { return m_pMeshFilter->m_AnimationClips.size(); }
	std::wstring GetClipName() const { return m_ClipSet?m_CurrentClip.Name:L""; }
	std::vector<DirectX::XMFLOAT4X4> GetBoneTransforms() const { return m_Transforms; }

private:
	AnimationKey m_LastKey;
	AnimationClip m_CurrentClip;
	MeshFilter* m_pMeshFilter;
	std::vector<DirectX::XMFLOAT4X4> m_Transforms, m_LastTransform;
	bool m_IsPlaying, m_Reversed, m_ClipSet, m_Transition, m_WaitForFinish, m_IsFinished;
	float m_TickCount, m_AnimationSpeed, m_TransitionLength, m_TransitionTimer;
};

