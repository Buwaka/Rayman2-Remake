#include "stdafx.h"
#include "ModelComponent.h"
#include "ContentManager.h"
#include "MeshFilter.h"
#include "Material.h"
#include "ModelAnimator.h"
#include "TransformComponent.h"
#include "PhysxManager.h"

ModelComponent::ModelComponent(std::wstring assetFile, bool castShadows):
	m_AssetFile(std::move(assetFile)),
	m_CastShadows(castShadows)
{
	m_pMeshFilter = ContentManager::Load<MeshFilter>(m_AssetFile);
}

ModelComponent::~ModelComponent()
{
	SafeDelete(m_pAnimator);

	if(m_CollissionMesh)
		m_CollissionMesh->release();
}

physx::PxTriangleMesh * ModelComponent::CreateCollissionMesh()
{
	if (!m_CollissionMesh)
	{
		auto physX = PhysxManager::GetInstance()->GetPhysics();
		auto cooking = PhysxManager::GetInstance()->GetCooking();

		physx::PxTolerancesScale scale;
		physx::PxCookingParams params(scale);

		// disable mesh cleaning - perform mesh validation on development configurations
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		// disable edge precompute, edges are set for each triangle, slows contact generation
		params.meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
		// lower hierarchy for internal mesh
		params.meshCookingHint = physx::PxMeshCookingHint::eCOOKING_PERFORMANCE;

		cooking->setParams(params);

		auto vertexcount = m_pMeshFilter->m_VertexCount;
		physx::PxVec3* vertices = new physx::PxVec3[vertexcount];

		for (size_t i = 0; i < vertexcount; i++)
		{
			physx::PxVec3 temp;
			temp.x = m_pMeshFilter->m_Positions[i].x;
			temp.y = m_pMeshFilter->m_Positions[i].y;
			temp.z = m_pMeshFilter->m_Positions[i].z;
			vertices[i] = temp;
		}

		physx::PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = m_pMeshFilter->m_VertexCount;
		meshDesc.points.stride = sizeof(physx::PxVec3);
		meshDesc.points.data = vertices;

		meshDesc.triangles.count = m_pMeshFilter->m_IndexCount / 3;
		meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
		meshDesc.triangles.data = m_pMeshFilter->m_Indices.data();




#ifdef _DEBUG

		if (!meshDesc.isValid())
		{
			Logger::LogWarning(L"ModelComponent::CreateCollissionMesh() > meshDesc is not valid");
		}

#endif

		m_CollissionMesh = cooking->createTriangleMesh(meshDesc, physX->getPhysicsInsertionCallback());

		delete vertices;
	}
	return m_CollissionMesh;
}

TextureData * ModelComponent::GetMeshTexture()
{
	return m_pMeshFilter->m_SourceTexture;
}

void ModelComponent::SetMaterial(UINT materialId)
{
	m_MaterialSet = true;
	m_MaterialId = materialId;
}

std::wstring ModelComponent::GetMeshName()
{
	return m_pMeshFilter->m_MeshName;
}

void ModelComponent::Initialize(const GameContext& gameContext)
{

	m_pMeshFilter->BuildIndexBuffer(gameContext);

	if (m_pMeshFilter->m_HasAnimations)
		m_pAnimator = new ModelAnimator(m_pMeshFilter);

	if (m_CastShadows)
	{
		gameContext.pShadowMapper->UpdateMeshFilter(gameContext, m_pMeshFilter);
	}

	UpdateMaterial(gameContext);
};

inline void ModelComponent::UpdateMaterial(const GameContext& gameContext)
{
	if (m_MaterialSet)
	{
		m_MaterialSet = false;

		//FORWARD
		const auto mat = gameContext.pMaterialManager->GetMaterial(m_MaterialId);
		if (mat == nullptr)
		{
			Logger::LogFormat(LogLevel::Warning, L"ModelComponent::UpdateMaterial > Material with ID \"%i\" doesn't exist!",
			                  m_MaterialId);
			return;
		}

		m_pMaterial = mat;
		m_pMaterial->Initialize(gameContext);
		m_pMeshFilter->BuildVertexBuffer(gameContext, m_pMaterial);
	}
}

void ModelComponent::Update(const GameContext& gameContext)
{
	UpdateMaterial(gameContext);

	if (m_pAnimator)
		m_pAnimator->Update(gameContext);
};

void ModelComponent::DrawShadowMap(const GameContext& gameContext)
{
	if (m_pAnimator)
		gameContext.pShadowMapper->Draw(gameContext, m_pMeshFilter, GetTransform()->GetWorld(), m_pAnimator->GetBoneTransforms());
	else
		gameContext.pShadowMapper->Draw(gameContext, m_pMeshFilter, GetTransform()->GetWorld());
};

void ModelComponent::Draw(const GameContext& gameContext)
{
	if (!m_pMaterial)
	{
		Logger::LogWarning(L"ModelComponent::Draw() > No Material!");
		return;
	}

	m_pMaterial = gameContext.pMaterialManager->GetMaterial(m_MaterialId);
	m_pMaterial->SetEffectVariables(gameContext, this);

	//Set Inputlayout
	gameContext.pDeviceContext->IASetInputLayout(m_pMaterial->GetInputLayout());

	//Set Vertex Buffer
	UINT offset = 0;
	auto vertexBufferData = m_pMeshFilter->GetVertexBufferData(gameContext, m_pMaterial);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride,
	                                               &offset);

	//Set Index Buffer
	gameContext.pDeviceContext->IASetIndexBuffer(m_pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set Primitive Topology
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//DRAW
	auto tech = m_pMaterial->GetTechnique();
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		tech->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_pMeshFilter->m_IndexCount, 0, 0);
	}
};
