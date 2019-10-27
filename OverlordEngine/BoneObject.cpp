#include "stdafx.h"
#include "BoneObject.h"
#include "MeshFilter.h"
#include "ContentManager.h"
#include "ModelAnimator.h"



BoneObject::BoneObject(int boneId, int materialId, float length)
	:m_BoneId{ boneId }
	, m_MaterialId{ materialId }
	, m_Length{ length }
{
	GetTransform()->Scale(m_Length, m_Length, m_Length);
}

void BoneObject::AddBone(BoneObject* pBone)
{
	//auto trans = GetTransform();
	//auto pos = DirectX::XMLoadFloat3(&trans->GetPosition());

	//auto temp = DirectX::XMFLOAT4(0, 0, m_Length,0);
	//DirectX::XMVECTOR length = DirectX::XMLoadFloat4(&temp);
	//auto worldMatrix = DirectX::XMLoadFloat4x4(&trans->GetWorld());


	//auto ttemp = DirectX::XMVector4Transform(length, worldMatrix);

	//DirectX::XMFLOAT3 transformed;
	//DirectX::XMStoreFloat3(&transformed, DirectX::XMVectorAdd(pos, ttemp));


	//pBone->GetTransform()->Translate(transformed);
	//pBone->GetTransform()->Rotate(DirectX::XMLoadFloat4(&trans->GetRotation()), true);
	//pBone->GetTransform()->Scale(trans->GetScale());
	auto newscale = pBone->m_Length / m_Length;
	pBone->GetTransform()->Scale(newscale, newscale, newscale);
	pBone->GetTransform()->Translate(0.f, 0.f, GetTransform()->GetPosition().z - (pBone->m_Length / m_Length));


	//pBone->GetTransform()->Translate(trans->GetPosition() + )

	AddChild(pBone);
}

void BoneObject::SetLength(float length)
{
	m_Length = length;
}

float BoneObject::GetLength()
{
	return m_Length;
}

DirectX::XMFLOAT4X4 BoneObject::GetBindPose() const
{
	return m_BindPose;
}

void BoneObject::CalculateBindPose()
{
	_CalculateBindPose();

	for (auto boneChild : GetChildren<BoneObject>())
	{
		boneChild->_CalculateBindPose();
	}
}

void BoneObject::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	m_Bone = new ModelComponent(BONEMESH);
	m_Bone->SetMaterial(m_MaterialId);

	AddComponent(m_Bone);
}

void BoneObject::_CalculateBindPose()
{
	DirectX::XMFLOAT4X4 worldMatrix = GetTransform()->GetWorld();
	DirectX::XMMATRIX inverseWorld = DirectX::XMLoadFloat4x4(&worldMatrix);
	inverseWorld = DirectX::XMMatrixInverse(nullptr, inverseWorld);
	DirectX::XMStoreFloat4x4(&m_BindPose, inverseWorld);
}
