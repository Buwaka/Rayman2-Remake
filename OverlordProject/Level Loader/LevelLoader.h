#pragma once

#include "ModelComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"

class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader();

	void Open(std::string folder, std::string folter = ".obj");

	std::vector<ModelComponent*>& GetMeshes();
	std::vector<ModelComponent*>& GetPanningMeshes();
	std::vector<ColliderComponent*>& GetCollissionMeshes();
	std::vector<DirectX::XMFLOAT3>& GetLums() { return m_Lums; };
	std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT2>>& GetClimbingAreas() { return m_Climbing; };
	DirectX::XMFLOAT3 GetStartPosition() { return m_StartPosition; };
	std::pair<DirectX::XMFLOAT3, float>& GetEndPosition() { return m_EndPosition; };


private:
	enum levelscriptobjs
	{
		fooliage,
		panning,
		startposition,
		endposition,
		climb,
		lum,
		junk
	};

	levelscriptobjs ParseScriptHeader(std::string in);

	std::vector<std::string> m_Fooliage;
	std::vector<std::string> m_Panning;
	std::vector<std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT2>> m_Climbing;
	std::vector<DirectX::XMFLOAT3> m_Lums;
	DirectX::XMFLOAT3 m_StartPosition{};
	std::pair<DirectX::XMFLOAT3, float> m_EndPosition;

	std::vector<ModelComponent*> m_Meshes;
	std::vector<ModelComponent*> m_PanningMeshes;
	std::vector<ColliderComponent*> m_ColMeshes;
};

