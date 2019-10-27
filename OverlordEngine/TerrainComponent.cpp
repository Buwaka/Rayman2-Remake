#include "stdafx.h"

#include "TerrainComponent.h"
 #include <utility>
#include "ColliderComponent.h"
#include "ContentManager.h"
#include "EffectHelper.h"
#include "TransformComponent.h"
#include "TextureData.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "GameScene.h"
#include "RigidBodyComponent.h"


TerrainComponent::TerrainComponent(std::wstring heightMapFile, std::wstring textureFile,
                                   unsigned int nrOfRows, unsigned int nrOfColumns, float width, float depth,
                                   float maxHeight) :
	m_HeightMapFile(std::move(heightMapFile)),
	m_TextureFile(std::move(textureFile)),
	m_NrOfRows(nrOfRows),
	m_NrOfColumns(nrOfColumns),
	m_NrOfVertices(nrOfRows * nrOfColumns),
	m_Width(width),
	m_Depth(depth),
	m_MaxHeight(maxHeight)
{}

TerrainComponent::~TerrainComponent()
{
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
}

void TerrainComponent::Initialize(const GameContext& gameContext)
{
	//Load Effect
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"Resources/Effects/PosNormTex3D.fx");
	m_pTechnique = m_pEffect->GetTechniqueByIndex(0);

	//TODO: Error handling
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorld")->AsMatrix();
	m_pDiffuseVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();

	UINT ilSize = 0;
	if (!EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, ilSize))
		Logger::LogError(L"TerrainComponent::Initialize >> BuildInputLayout failed!");

	//Texture
	m_pTextureData = ContentManager::Load<TextureData>(m_TextureFile);

	//Load Height Map
	ReadHeightMap();

	//Create Vertices & Triangles
	CalculateVerticesAndIndices();

	if (!m_VecVertices.empty())
	{
		//Build Vertexbuffer
		BuildVertexBuffer(gameContext);
		//Build Indexbuffer
		BuildIndexBuffer(gameContext);

		//Create PhysX Heightfield
		CreatePxHeightField();
	}
}

void TerrainComponent::Update(const GameContext& )
{}

void TerrainComponent::Draw(const GameContext& gameContext)
{
	if (m_VecVertices.empty())
		return;

	DirectX::XMMATRIX world = XMLoadFloat4x4(&GetTransform()->GetWorld());
	const DirectX::XMMATRIX viewProj = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjection());
	DirectX::XMMATRIX wvp = XMMatrixMultiply(world, viewProj);
	m_pMatWorldVariable->SetMatrix(reinterpret_cast<float*>(&world));
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<float*>(&wvp));
	m_pDiffuseVariable->SetResource(m_pTextureData->GetShaderResourceView());

	// Set vertex buffer
	const unsigned int stride = sizeof(VertexPosNormTex);
	unsigned int offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set index buffer
	gameContext.pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the input layout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);

	// Set primitive topology
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render a cube
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
	
}

//Exercise - Heightmap
void TerrainComponent::ReadHeightMap()
{
	m_VecHeightValues.resize(m_NrOfVertices, 0);

	//TODO: complete
	std::ifstream inFile;
	inFile.open(m_HeightMapFile.c_str(), std::ios_base::binary);
	if (!inFile)
	{
		Logger::LogFormat(LogLevel::Error, L"Loading terrain \'%ls\' failed!", m_HeightMapFile.c_str());
		return;
	}

	inFile.read(reinterpret_cast<char*>(&m_VecHeightValues[0]), static_cast<std::streamsize>(m_NrOfVertices * sizeof(unsigned short)));
	inFile.close();

}

//Exercise - Flat Grid
void TerrainComponent::CalculateVerticesAndIndices()
{
	using namespace DirectX;

	//TODO: complete
	//**VERTICES
	

	float cellWidth = m_Width / m_NrOfColumns;
	float cellDepth = m_Width / m_NrOfColumns;
	float cellXpos = -m_Width / 2.f;
	float cellZpos = m_Depth / 2.f;

	for(unsigned int row{};row < m_NrOfRows;++row)
	{
		cellXpos = -m_Width / 2.f;
		for(unsigned int col = 0; col<m_NrOfColumns;++col)
		{
			m_VecVertices.push_back({});//=================================================
			int vertexId = row * m_NrOfColumns + col;
			m_VecVertices[vertexId].Position.x = cellXpos;
			m_VecVertices[vertexId].Position.z = cellZpos;
			//float test = 50.f/USHRT_MAX;
			//test = (float(m_VecHeightValues.at(vertexId)) / float(USHRT_MAX));
			//test = (float(m_VecHeightValues.at(vertexId)) / float(USHRT_MAX))* m_MaxHeight;
			m_VecVertices[vertexId].Position.y = (float(m_VecHeightValues.at(vertexId)) / USHRT_MAX) * m_MaxHeight;

			m_VecVertices[vertexId].TexCoord.x = float(col) / float(m_NrOfColumns - 1);
			m_VecVertices[vertexId].TexCoord.y = float(row) / float(m_NrOfRows - 1);

			cellXpos += cellWidth;
		}
		cellZpos -= cellDepth;
	}

	int nrQuadsRow = m_NrOfRows - 1;
	int nrQuadsCollumns = m_NrOfColumns - 1;

	for(int row=0; row< nrQuadsRow;++row)
	{
		for(int col=0;col < nrQuadsCollumns;++col)
		{
			int a = row * m_NrOfColumns + col;
			int b = a + 1;
			int c = a + m_NrOfColumns;
			int d = c + 1;
			AddQuad(a, b, c, d);
		}
	}

	for(size_t i = 0;i< m_VecIndices.size();i+=6)
	{
		XMVECTOR a{}, b{}, c{}, d{}, e{}, f{};
		a = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i + 0)).Position);
		b = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i + 1)).Position);
		c = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i + 2)).Position);
		d = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i + 3)).Position);
		e = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i + 4)).Position);
		f = XMLoadFloat3(&m_VecVertices.at(m_VecIndices.at(i + 5)).Position);

		//first traingle
		XMVECTOR v1, v2, normal;
		v1 = c - a;
		v2 = b - a;
		normal = XMVector3Cross(v1, v2);
		normal = XMVector3Normalize(normal);
		XMFLOAT3 normalFloat3;
		XMStoreFloat3(&normalFloat3, normal);
		m_VecFaceNormals.push_back(normalFloat3);

		//second triangle
		v1 = e - f;
		v2 = d - f;
		normal = XMVector3Cross(v1, v2);
		normal = XMVector3Normalize(normal);
		XMStoreFloat3(&normalFloat3, normal);
		m_VecFaceNormals.push_back(normalFloat3);
	}


	

	int numFacesPerRow = (m_NrOfColumns - 1) * 2;
	//XMFLOAT3 normals[6];
	int index[6];
	//from left back to right front
	for(unsigned int row =0;row < m_NrOfRows;++row)
	{
		for(unsigned int col = 0; col < m_NrOfColumns;++col)
		{
			int centerindex = numFacesPerRow * row + col * 2;
			index[0] = centerindex - 1;
			index[1] = centerindex;
			index[2] = centerindex + 1;
			index[3] = centerindex - numFacesPerRow - 2;
			index[4] = centerindex - numFacesPerRow - 1;
			index[5] = centerindex - numFacesPerRow - 0;

			if (col == 0)
			{
				index[0] = -1;
				index[3] = -1;
				index[4] = -1;
			}

			if(col==m_NrOfColumns-1)
			{
				index[1] = -1;
				index[2] = -1;
				index[5] = -1;
			}

			XMVECTOR sum{};
			for(int i=0;i<6;++i)
			{
				if ((index[i] >= 0) && (index[i] < (int)m_VecFaceNormals.size()))
				{
					sum += XMLoadFloat3(&m_VecFaceNormals.at(index[i]));
				}
			}
			sum = XMVector3Normalize(sum);
			int vertexId = row * m_NrOfColumns + col;
			XMStoreFloat3(&m_VecVertices[vertexId].Normal, sum);
			
		}
	}
}

//Exercise - Flat Grid
void TerrainComponent::AddTriangle(const unsigned int a, const unsigned int b, const unsigned c)
{

	m_VecIndices.push_back(a);
	m_VecIndices.push_back(b);
	m_VecIndices.push_back(c);
}

//Exercise - Flat Grid
void TerrainComponent::AddQuad(const unsigned int a, const unsigned int b, const unsigned c, const unsigned d)
{
	AddTriangle(a, d, c);
	AddTriangle(a,b, d);
}

//Exercise - PhysX
void TerrainComponent::CreatePxHeightField()
{
	//https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/3.3.4/Manual/Geometry.html#geometry-types

	//TODO: Implement PhysX HeightField
	//After implementing the physX representation - Fix the lagginess from the PhysX Debugger! (If any)

	physx::PxHeightFieldSample* pSamples = new physx::PxHeightFieldSample[m_NrOfVertices]{};
	for (unsigned int i = 0, j = m_VecVertices.size(); i < j; ++i)
	{
		pSamples[i].height = physx::PxI16(m_VecVertices[i].Position.y);
	}

	physx::PxHeightFieldDesc hfDesc;
	hfDesc.format = physx::PxHeightFieldFormat::eS16_TM;
	hfDesc.nbRows = m_NrOfRows;
	hfDesc.nbColumns = m_NrOfColumns;
	hfDesc.samples.data = pSamples;
	hfDesc.samples.stride = sizeof(physx::PxHeightFieldSample);

	auto physX = PhysxManager::GetInstance()->GetPhysics();

	physx::PxMaterial* pMat = physX->createMaterial(0.5f, 0.5f, 0.1f);
	physx::PxHeightField* pHeightField = physX->createHeightField(hfDesc);
	std::shared_ptr<physx::PxGeometry> pGeo{ new physx::PxHeightFieldGeometry(pHeightField, {}, 1.0f, m_Width / m_NrOfRows, m_Depth / m_NrOfColumns) };

	ColliderComponent* pCollider = new ColliderComponent(pGeo, *pMat);

	GameObject* pChild = new GameObject();
	pChild->AddComponent(pCollider);
	pChild->AddComponent(new RigidBodyComponent(true));

	if (m_pGameObject != nullptr) m_pGameObject->AddChild(pChild);

	pChild->GetTransform()->Rotate(DirectX::XMFLOAT3(0, 90.0f, 0));
	pChild->GetTransform()->Translate(-m_Width * 0.5f, 0, m_Depth*0.5f);
	delete[] pSamples;
}

void TerrainComponent::BuildVertexBuffer(const GameContext& gameContext)
{
	//Vertexbuffer
	D3D11_BUFFER_DESC bd = {};
	D3D11_SUBRESOURCE_DATA initData = {nullptr};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(VertexPosNormTex) * m_NrOfVertices;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = m_VecVertices.data();
	const HRESULT hr = gameContext.pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	Logger::LogHResult(hr, L"Failed to Create Vertexbuffer");
}

void TerrainComponent::BuildIndexBuffer(const GameContext& gameContext)
{
	D3D11_BUFFER_DESC bd = {};
	D3D11_SUBRESOURCE_DATA initData = {nullptr};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(DWORD) * m_VecIndices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = m_VecIndices.data();
	const HRESULT hr = gameContext.pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	Logger::LogHResult(hr, L"Failed to Create Indexbuffer");
	m_NumIndices = m_VecIndices.size();
}
