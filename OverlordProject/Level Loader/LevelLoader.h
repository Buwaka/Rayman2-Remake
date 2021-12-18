#pragma once

#include "ModelComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"

//example script
//fooliage, Learn_10_0x466237C_11, Learn_10_0x469BF7C_0, Learn_10_0x469DE04_0, Learn_10_0x469B008_0, Learn_10_0x469CFE8_0, Learn_10_0x469DE04_0, Learn_10_0x469BB60_0, Learn_10_0x46842A8_12, Learn_10_0x4678D28_5, Learn_10_0x467F1DC_12, Learn_10_0x468C434_12, Learn_10_0x466C0C8_4, Learn_10_0x465CF5C_5, Learn_10_0x4696188_0, Learn_10_0x466AE10_2, Learn_10_0x46842A8_5, Learn_10_0x46BEA7C_1, Learn_10_0x46BEA7C_0, Learn_10_0x46C3C2C_0, Learn_10_0x46C175C_0, Learn_10_0x46B6D14_0, Learn_10_0x46B9BD0_0, Learn_10_0x4678D28_13
//panning, Learn_10_0x46BEA7C_1, Learn_10_0x46BEA7C_0, Learn_10_0x46C3C2C_0, Learn_10_0x46C175C_0, Learn_10_0x46B6D14_0, Learn_10_0x46B9BD0_0
//startposition, 50, 0, 290
//endposition, 24, 23, 92.0, 12
//climb, 21.25, -9, 149, 0.85, 30
//lum, -15.6130161, -8.56460667, 291.521606
//lum, 4.70098639, -6.13700342, 310.165741
//lum, -89.2227097, -15.6971359, 266.162384
//lum, -47.3695221, -6.86403275, 211.505020
//lum, -14.4013138, -2.33713436, 195.968430
//
//
//#fooliage, <name>, <name>, ...
//#panning, <name>, <name>, ...
//#startposition, x, y, z
//#endposition, x, y, z, radius
//#climb, x, y, z, width, height
//#lum, x, y, z


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

