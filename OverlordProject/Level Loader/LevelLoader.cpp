#include "stdafx.h"
#include "LevelLoader.h"
#include "../Materials/DiffuseMaterial.h"
#include "GameObject.h"
#include <filesystem>
#include "PhysxManager.h"
#include "TextReader.h"
namespace fs = std::filesystem;


void LevelLoader::Open(std::string folder, std::string filter)
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	auto colMaterial = physX->createMaterial(0, 0, 0);

	auto script = new TextReader();
	script->Open(folder + "script.scp");


	if (script->Exists())
	{
		std::string line = script->ReadLine();
		bool eof = false;
		while (!eof)
		{
			std::vector<std::string> tokens;
			tokens = tokenize(line, ',');

			if (tokens.size() > 0)
			{
				levelscriptobjs type = ParseScriptHeader(tokens[0]);

				switch (type)
				{
				case levelscriptobjs::fooliage:
					for (size_t i = 1; i < tokens.size(); i++)
					{
						m_Fooliage.push_back(tokens[i]);
					}
					break;
				case levelscriptobjs::panning:
					for (size_t i = 1; i < tokens.size(); i++)
					{
						m_Panning.push_back(tokens[i]);
					}
					break;
				case levelscriptobjs::startposition:
					m_StartPosition.x = std::stof(tokens[1]);
					m_StartPosition.y = std::stof(tokens[2]);
					m_StartPosition.z = std::stof(tokens[3]);
					break;
				case levelscriptobjs::endposition:
					m_EndPosition.first.x = std::stof(tokens[1]);
					m_EndPosition.first.y = std::stof(tokens[2]);
					m_EndPosition.first.z = std::stof(tokens[3]);
					m_EndPosition.second = std::stof(tokens[4]);
					break;
				case levelscriptobjs::climb:
				{
					std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT2> temp;
					temp.first.x = std::stof(tokens[1]);
					temp.first.y = std::stof(tokens[2]);
					temp.first.z = std::stof(tokens[3]);
					temp.second.x = std::stof(tokens[4]);
					temp.second.y = std::stof(tokens[5]);
					m_Climbing.push_back(temp);
				}
				break;
				case levelscriptobjs::lum:
					DirectX::XMFLOAT3 mlum;
					mlum.x = std::stof(tokens[1]);
					mlum.y = std::stof(tokens[2]);
					mlum.z = std::stof(tokens[3]);
					m_Lums.push_back(mlum);
					break;
				default:
					break;
				}
			}

			if (!script->eof())
				line = script->ReadLine();
			else
				eof = true;
		}
	}


	for (auto& p : fs::recursive_directory_iterator(folder))
	{

		if (p.path().extension() == filter)
		{
#if _DEBUG
			std::cout << p.path() << '\n';
#endif // _DEBUG

			auto model = new ModelComponent(p.path());

			if (std::find(m_Panning.begin(), m_Panning.end(), To_stringSimple(model->GetMeshName())) == m_Panning.end())
			{
				m_Meshes.push_back(model);
			}
			else
			{
				m_PanningMeshes.push_back(model);
			}



			if (std::find(m_Fooliage.begin(), m_Fooliage.end(), To_stringSimple(model->GetMeshName())) == m_Fooliage.end())
			{
				auto geo = model->CreateCollissionMesh();
				std::shared_ptr<physx::PxGeometry> meshgeo(new physx::PxTriangleMeshGeometry(geo));
				auto col = new ColliderComponent(meshgeo, *colMaterial);
				m_ColMeshes.push_back(col);
			}
		}
	}

	delete script;

}

LevelLoader::LevelLoader()
{
}

LevelLoader::~LevelLoader()
{
}

std::vector<ModelComponent*>& LevelLoader::GetMeshes()
{
	return m_Meshes;
}

std::vector<ModelComponent*>& LevelLoader::GetPanningMeshes()
{
	return m_PanningMeshes;
}

std::vector<ColliderComponent*>& LevelLoader::GetCollissionMeshes()
{
	return m_ColMeshes;
}

LevelLoader::levelscriptobjs LevelLoader::ParseScriptHeader(std::string in)
{
	std::string token = ToUpper(in);
	levelscriptobjs out = junk;

	if (in[0] == '#')
		return junk;

	if (token == "FOOLIAGE")
		out = fooliage;
	else if (token == "PANNING")
		out = panning;
	else if (token == "STARTPOSITION")
		out = startposition;
	else if (token == "ENDPOSITION")
		out = endposition;
	else if (token == "CLIMB")
		out = climb;
	else if (token == "LUM")
		out = lum;


	return out;
}
