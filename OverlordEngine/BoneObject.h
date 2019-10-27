#pragma once
#include "GameObject.h"
#include "ModelComponent.h"
#include "TransformComponent.h"

#define BONEMESH L"./Resources/Meshes/Bone.ovm"

class MeshFilter;

struct SkinnedVertex
{
	SkinnedVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT4 color, float blend1, float blend2) :
		TransformedVertex(position, normal, color), OriginalVertex(position, normal, color), BlendWeight1(blend1), BlendWeight2(blend2)
	{}
	VertexPosNormCol TransformedVertex;
	VertexPosNormCol OriginalVertex;
	float BlendWeight1, BlendWeight2;
};

class BoneObject final : public GameObject
{
public:  
	BoneObject(int boneId, int materialId, float length = 5.0f); 
	~BoneObject() = default;

	BoneObject(const BoneObject& other) = delete;  
	BoneObject(BoneObject&& other) noexcept = delete; 
	BoneObject& operator=(const BoneObject& other) = delete; 
	BoneObject& operator=(BoneObject&& other) noexcept = delete;

	
	void AddBone(BoneObject* pBone);
	void SetLength(float length);
	float GetLength();

	DirectX::XMFLOAT4X4 GetBindPose() const;
	void CalculateBindPose();
	

protected:  
	virtual void Initialize(const GameContext& gameContext);
	//virtual void Draw(const GameContext&) override;
	
private: 
	void _CalculateBindPose();

	float m_Length;  
	int m_BoneId; 
	int m_MaterialId;

	ModelComponent* m_Bone;
	DirectX::XMFLOAT4X4 m_BindPose;
};