#include "stdafx.h"
#include "MeshFilterLoader.h"
#include "BinaryReader.h"
#include "TextReader.h"
#include "EffectHelper.h"
#include "ContentManager.h"
#include "TextureData.h"
#include <cctype>
#include <filesystem>
#include <iostream>

inline bool exists_test(const std::string& name) 
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}


#define OVM_vMAJOR 1
#define OVM_vMINOR 1



MeshFilter* MeshFilterLoader::LoadContent(const std::wstring& assetFile)
{
	std::wstring extension = getFileExt(assetFile);

	Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::LoadContent() > File extension \"%s\" found ", assetFile.c_str());

	if (ToWUpper(extension)  == L"OVM")
	{
		return OVMLoader(assetFile);
	}
	else if (ToWUpper(extension) == L"OBJ")
	{
		return OBJLoader(assetFile);
	}

	Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::LoadContent() > File extension \"%s\" not implemented ", assetFile.c_str());
	return nullptr;


}

void MeshFilterLoader::Destroy(MeshFilter* objToDestroy)
{
	SafeDelete(objToDestroy);
}

MeshFilter * MeshFilterLoader::OVMLoader(const std::wstring & assetFile)
{
	auto binReader = new BinaryReader();
	binReader->Open(assetFile);

	if (!binReader->Exists())
		return nullptr;

	//READ OVM FILE
	const int versionMajor = binReader->Read<char>();
	const int versionMinor = binReader->Read<char>();

	if (versionMajor != OVM_vMAJOR || versionMinor != OVM_vMINOR)
	{
		Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::Load() > Wrong OVM version\n\tFile: \"%s\" \n\tExpected version %i.%i, not %i.%i.", assetFile.c_str(), OVM_vMAJOR, OVM_vMINOR, versionMajor, versionMinor);
		delete binReader;
		return nullptr;
	}

	unsigned int vertexCount = 0;
	unsigned int indexCount = 0;

	auto pMesh = new MeshFilter();

	for (;;)
	{
		const auto meshDataType = static_cast<MeshDataTypeOVM>(binReader->Read<char>());
		if (meshDataType == MeshDataTypeOVM::END)
			break;

		const auto dataOffset = binReader->Read<unsigned int>();

		switch (meshDataType)
		{
		case MeshDataTypeOVM::HEADER:
		{
			pMesh->m_MeshName = binReader->ReadString();
			vertexCount = binReader->Read<unsigned int>();
			indexCount = binReader->Read<unsigned int>();

			pMesh->m_VertexCount = vertexCount;
			pMesh->m_IndexCount = indexCount;
		}
		break;
		case MeshDataTypeOVM::POSITIONS:
		{
			pMesh->m_HasElement |= ILSemantic::POSITION;

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 pos;
				pos.x = binReader->Read<float>();
				pos.y = binReader->Read<float>();
				pos.z = binReader->Read<float>();
				pMesh->m_Positions.push_back(pos);
			}
		}
		break;
		case MeshDataTypeOVM::INDICES:
		{
			for (unsigned int i = 0; i < indexCount; ++i)
			{
				pMesh->m_Indices.push_back(binReader->Read<DWORD>());
			}
		}
		break;
		case MeshDataTypeOVM::NORMALS:
		{
			pMesh->m_HasElement |= ILSemantic::NORMAL;

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 normal;
				normal.x = binReader->Read<float>();
				normal.y = binReader->Read<float>();
				normal.z = binReader->Read<float>();
				pMesh->m_Normals.push_back(normal);
			}
		}
		break;
		case MeshDataTypeOVM::TANGENTS:
		{
			pMesh->m_HasElement |= ILSemantic::TANGENT;

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 tangent;
				tangent.x = binReader->Read<float>();
				tangent.y = binReader->Read<float>();
				tangent.z = binReader->Read<float>();
				pMesh->m_Tangents.push_back(tangent);
			}
		}
		break;
		case MeshDataTypeOVM::BINORMALS:
		{
			pMesh->m_HasElement |= ILSemantic::BINORMAL;

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT3 binormal;
				binormal.x = binReader->Read<float>();
				binormal.y = binReader->Read<float>();
				binormal.z = binReader->Read<float>();
				pMesh->m_Binormals.push_back(binormal);
			}
		}
		break;
		case MeshDataTypeOVM::TEXCOORDS:
		{
			pMesh->m_HasElement |= ILSemantic::TEXCOORD;

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT2 tc;
				tc.x = binReader->Read<float>();
				tc.y = binReader->Read<float>();
				pMesh->m_TexCoords.push_back(tc);
			}
		}
		break;
		case MeshDataTypeOVM::COLORS:
		{
			pMesh->m_HasElement |= ILSemantic::COLOR;

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMFLOAT4 color;
				color.x = binReader->Read<float>();
				color.y = binReader->Read<float>();
				color.z = binReader->Read<float>();
				color.w = binReader->Read<float>();
				pMesh->m_Colors.push_back(color);
			}
		}
		break;
		case MeshDataTypeOVM::BLENDINDICES:
		{
			pMesh->m_HasElement |= ILSemantic::BLENDINDICES;

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				DirectX::XMUINT4 blendIndex;
				blendIndex.x = (unsigned int)binReader->Read<float>();
				blendIndex.y = (unsigned int)binReader->Read<float>();
				blendIndex.z = (unsigned int)binReader->Read<float>();
				blendIndex.w = (unsigned int)binReader->Read<float>();
				pMesh->m_BlendIndices.push_back(blendIndex);
			}

			//TODO: Start parsing the BlendIndices for every vertex
			//and add them to the corresponding vector
			//pMesh->m_BlendIndices
		}
		break;
		case MeshDataTypeOVM::BLENDWEIGHTS:
		{
			pMesh->m_HasElement |= ILSemantic::BLENDWEIGHTS;

			for (unsigned int i = 0; i < vertexCount; ++i)
			{
				//read as float cuz >>>format 
				DirectX::XMFLOAT4 blendWeights;
				blendWeights.x = binReader->Read<float>();
				blendWeights.y = binReader->Read<float>();
				blendWeights.z = binReader->Read<float>();
				blendWeights.w = binReader->Read<float>();
				pMesh->m_BlendWeights.push_back(blendWeights);
			}

			//TODO: Start parsing the BlendWeights for every vertex
			//and add them to the corresponding vector
			//pMesh->m_BlendWeights
		}
		break;
		case MeshDataTypeOVM::ANIMATIONCLIPS:
		{
			pMesh->m_HasAnimations = true;

			size_t clipCount = 0;
			//TODO: Start parsing the AnimationClips
			//1. Read the clipCount
			clipCount = binReader->Read<uint16_t>();
			pMesh->m_AnimationClips.reserve(clipCount);

			for (size_t i = 0; i < clipCount; i++)
			{
				AnimationClip AClip;
				AClip.Name = binReader->ReadString();
				AClip.Duration = binReader->Read<float>();
				AClip.TicksPerSecond = binReader->Read<float>();

				uint16_t KeySize = binReader->Read<uint16_t>();
				AClip.Keys.reserve(KeySize);

				for (size_t j = 0; j < KeySize; j++)
				{
					AnimationKey AKey;
					AKey.Tick = binReader->Read<float>();

					uint16_t BTransSize = binReader->Read<uint16_t>();
					AKey.BoneTransforms.reserve(BTransSize);

					for (size_t k = 0; k < BTransSize; k++)
					{
						DirectX::XMFLOAT4X4 temp;
						temp = binReader->Read<DirectX::XMFLOAT4X4>();
						AKey.BoneTransforms.push_back(temp);
					}
					AClip.Keys.push_back(AKey);
				}
				//sort keys
				std::sort(AClip.Keys.begin(), AClip.Keys.end(), [](AnimationKey &A, AnimationKey &B) {return A.Tick < B.Tick; });

				pMesh->m_AnimationClips.push_back(AClip);
			}
			//2. For every clip
			//3. Create a AnimationClip object (clip)
			//4. Read/Assign the ClipName
			//5. Read/Assign the ClipDuration
			//6. Read/Assign the TicksPerSecond
			//7. Read the KeyCount for this clip
			//8. For every key
			//9. Create a AnimationKey object (key)
			//10. Read/Assign the Tick
			//11. Read the TransformCount
			//12. For every transform
			//13. Create a XMFLOAT4X4
			//14. The following 16 floats are the matrix values, they are stored by row
			// float0 = readFloat (ROW1) (_11)
			// float1 = readFloat (ROW1) (_12)
			// float2 = readFloat (ROW1)
			// float3 = readFloat (ROW1)
			// float4 = readFloat (ROW2) (_21)
			//...
			// float15 = readFloat (ROW4) (_44)
			//
			//MATRIX:
			// [ float0	float1	float2	float3 ]
			// [ float4	...		...		...	   ]
			// [ ...	...		...		...	   ]
			// [ ...	...		...		float15]
			//15. Add The matrix to the BoneTransform vector of the key
			//16. Add the key to the key vector of the clip
			//17. Add the clip to the AnimationClip vector of the MeshFilter (pMesh->m_AnimationClips)
		}
		break;
		case MeshDataTypeOVM::SKELETON:
		{
			//TODO: Complete
			//1. Read/Assign the boneCount (pMesh->m_BoneCount)
			//2. Move the buffer to the next block position (don't forget that we already moved the reader ;) )
			pMesh->m_BoneCount = binReader->Read<uint16_t>();
			binReader->MoveBufferPosition(dataOffset - sizeof(uint16_t));
		}
		break;
		default:
			binReader->MoveBufferPosition(dataOffset);
			break;
		}
	}

	delete binReader;

	return pMesh;
}

//v,
//vt,
//vn,
//vp,
//f,
//o,
//g,
//s,
//mtllib,
//usemtl

MeshFilterLoader::MeshDataTypeOBJ MeshFilterLoader::GetOBJType(std::string type)
{
	std::string token = ToUpper(type);
	if (token == "V")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::v;
	}
	else if (token == "VT")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::vt;
	}
	else if (token == "VN")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::vn;
	}
	else if (token == "VP")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::vp;
	}
	else if (token == "F")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::f;
	}
	else if (token == "O")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::o;
	}
	else if (token == "G")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::g;
	}
	else if (token == "S")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::s;
	}
	else if (token == "MTLLIB")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::mtllib;
	}
	else //(token == "USEMTL")
	{
		return MeshFilterLoader::MeshDataTypeOBJ::usemtl;
	}
}

MeshFilter * MeshFilterLoader::OBJLoader(const std::wstring & assetFile)
{
	//NOTE: OBJ files should be internally sorted so that v, vt and vn entries are before the f lines, as f lines make reference to previous v, vt and vn entries. 
	//Henceforth this parser requires the OBJ files to be internally sorted, but since the rayman 2 static maps export are already sorted, I will omit the sorting task alltogether

	auto textReader = new TextReader();
	textReader->Open(To_stringSimple(assetFile));

	if (!textReader->Exists())
	{
		delete textReader;
		return nullptr;
	}


	auto pMesh = new MeshFilter();
	pMesh->m_Indices.reserve(200);
	pMesh->m_TexCoords.reserve(200);
	pMesh->m_Positions.reserve(200);
	std::vector<DirectX::XMFLOAT2> texcoordLUT;
	texcoordLUT.reserve(200);
	std::vector<DirectX::XMFLOAT3> vectorLUT;
	vectorLUT.reserve(200);
	int indexcounter = 0;




	std::string line = textReader->ReadLine();
	bool eof = false;
	while (!eof)
	{

		std::vector<std::string> tokens;
		tokens = tokenize(line,' ');


		MeshDataTypeOBJ type = GetOBJType(tokens[0]);

		switch (type)
		{
		case MeshFilterLoader::MeshDataTypeOBJ::v:
		{
			pMesh->m_HasElement |= ILSemantic::POSITION;
			DirectX::XMFLOAT3 pos;

			pos.x = - std::stof(tokens[1]);
			pos.y = std::stof(tokens[2]);
			pos.z = std::stof(tokens[3]);
			//a 4th W coordinate could be present, but is not used in rayman 2 as far as I can see
			vectorLUT.push_back(pos);
		}
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::vt:
		{
			pMesh->m_HasElement |= ILSemantic::TEXCOORD;
			DirectX::XMFLOAT2 pos;

			pos.x = std::stof(tokens[1]);
			pos.y = - std::stof(tokens[2]);
			//pos.z = std::stof(tokens[3]);
			//a 3th W coordinate could be present, but is not used in rayman 2 as far as I can see

			texcoordLUT.push_back(pos);
		}
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::vn:
			//omitted because not used

			//pMesh->m_HasElement |= ILSemantic::NORMAL;
			//DirectX::XMFLOAT3 pos;

			//pos.x = std::stof(tokens[1]);
			//pos.y = std::stof(tokens[2]);
			//pos.z = std::stof(tokens[3]);

			//pMesh->m_Normals.push_back(pos);
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::vp:
			//omitted
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::f:

			for (size_t i = 1; i < tokens.size(); i++)
			{
				std::vector<std::string> subtokens;
				subtokens = tokenize(tokens[i], '/');

				int index = std::stoi(subtokens[0]) - 1;

				pMesh->m_Positions.push_back(vectorLUT[index]);
				pMesh->m_Indices.push_back(indexcounter);
				indexcounter++;
				if (subtokens.size() > 1)
				{
					int texcoordindex = std::stoi(subtokens[1]) -1;
					pMesh->m_TexCoords.push_back(texcoordLUT[texcoordindex]);
				}
				else
				{
					Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::LoadContent() > failed to load texture ", assetFile.c_str());
				}

			}


			break;
		case MeshFilterLoader::MeshDataTypeOBJ::o:
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::g:
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::s:
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::mtllib:
			//just grabbing the texture filepath, since only that's used
		{
			//std::wstring path = GetPath(assetFile);
			std::filesystem::path path(GetPath(assetFile));
			auto filename = tokens[1].substr(0, tokens[1].find("."));
			pMesh->m_MeshName = To_wstringSimple(filename);


			auto subtextReader = new TextReader();
			subtextReader->Open(path.generic_string() + tokens[1]);

			if (!subtextReader->Exists())
				break;

			bool found = false;
			std::filesystem::path mtlpath;
			std::string temp = subtextReader->ReadLine();
			bool seof = false;
			while (!seof)
			{
				int i = temp.find("map_Kd");
				int j = temp.find((char)0, i);
				if (i != std::string::npos)
				{
					mtlpath = temp.substr(i + 7, j);
					found = true;
					break;
				}
				if (!subtextReader->eof())
					temp = subtextReader->ReadLine();
				else
					seof = true;
			}


			if (found)
			{
				auto rpath = (path / mtlpath);
				auto ddstest = rpath;
				ddstest.replace_extension(".dds");

				while (pMesh->m_SourceTexture == nullptr)
				{
					if (exists_test(ddstest.generic_string()))
					{
						pMesh->m_SourceTexture = ContentManager::Load<TextureData>(ddstest);
					}
					else
					{
						pMesh->m_SourceTexture = ContentManager::Load<TextureData>(rpath);
					}
					if (pMesh->m_SourceTexture == nullptr)
						Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::LoadContent() > failed to load texture ", assetFile.c_str());
				}
			}
			delete subtextReader;
		}
			break;
		case MeshFilterLoader::MeshDataTypeOBJ::usemtl:
			//misc unused metadata
			break;
		default:
			break;
		}

		if (!textReader->eof())
			line = textReader->ReadLine();
		else
			eof = true;
	}


	std::reverse(pMesh->m_Indices.begin(), pMesh->m_Indices.end());
	//std::reverse(pMesh->m_Indices.begin(), pMesh->m_Indices.end());
	pMesh->m_VertexCount = pMesh->m_Positions.size();
	pMesh->m_IndexCount = pMesh->m_Indices.size();
	if(pMesh->m_VertexCount == 0)
		Logger::LogFormat(LogLevel::Warning, L"MeshDataLoader::LoadContent() > no vertices found ", assetFile.c_str());

	delete textReader;

	return pMesh;
}
