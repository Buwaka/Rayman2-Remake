#include "stdafx.h"

#include "RaymanGame.h"
#include "GameObject.h"
#include "Components.h"
#include "ContentManager.h"
#include "PhysxManager.h"
#include "Character.h"
#include "PhysxProxy.h"
#include "../Materials/ColorMaterial.h"
#include "../Materials/DiffuseMaterial.h"
#include "..//Level Loader/LevelLoader.h"
#include "PxTriangleMeshLoader.h"
#include "cooking/PxCooking.h"
#include "../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "../Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "../Materials/SkyBoxMaterial.h"
#include "SpriteFont.h"
#include "TextRenderer.h"
#include "OverlordGame.h"
#include "GeneralStructs.h"
#include "SceneManager.h"
#include "Lum.h"
#include "ClimbArea.h"
#include "EndPortal.h"
#include "SoundManager.h"
#include "../Materials/PP/PostGrain.h"
#include "..\Materials\Shadow\DiffuseMaterial_Shadow_Panning.h"


const float MAXHP = 5.0f;

RaymanGame::RaymanGame() :
	GameScene(L"RaymanGame"),
	m_pLevel(nullptr),
	m_pFont(nullptr),
	m_pPPMat(nullptr)
{}

RaymanGame::~RaymanGame()
{
	m_pLumSFX->release();
	m_pMusic->release();
	SafeDelete(m_pPPMat);
}



void RaymanGame::Initialize()
{
	m_MaxHP = MAXHP;
	m_HP = m_MaxHP;
	m_LumCount = m_CageCount = m_LumsCollected = m_CageCollected = 0;


	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Rayman2font.fnt");


	const auto gameContext = GetGameContext();
	gameContext.pShadowMapper->SetLight({ -98.8609543f , 176.190842f , 328.102081f }, { 0.357432812f, -0.894819140f, -0.267470717f });
	//GetPhysxProxy()->EnablePhysxDebugRendering(true);


	auto skyBox = new GameObject();

	auto *skyMat = new SkyBoxMaterial();
	skyMat->SetSkyBoxMat(L"Resources/Textures/foret_sky_txy.gf.png");

	auto id = gameContext.pMaterialManager->AddMaterial(skyMat);
	skyBox->AddComponent(new ModelComponent(L"Resources/Meshes/box.ovm"));
	skyBox->GetComponent<ModelComponent>()->SetMaterial(id);

	AddChild(skyBox);




	m_pLevel = new GameObject();

	m_LevelLoader.Open("./Resources/levels/Learn_10/");

	auto meshes = m_LevelLoader.GetMeshes();
	auto panningmeshes = m_LevelLoader.GetPanningMeshes();
	auto colMeshes = m_LevelLoader.GetCollissionMeshes();

	for (auto& mesh : meshes)
	{
		m_pLevel->AddComponent(mesh);
	}

	for (auto& mesh : panningmeshes)
	{
		m_pLevel->AddComponent(mesh);
	}

	for (auto& mesh : colMeshes)
	{
		m_pLevel->AddComponent(mesh);
	}

	m_pLevel->AddComponent(new RigidBodyComponent(true));

	AddChild(m_pLevel);

	for (auto& mesh : meshes)
	{
		auto dMat = new DiffuseMaterial_Shadow();
		dMat->SetDiffuseTexture(mesh->GetMeshTexture());
		dMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
		id = gameContext.pMaterialManager->AddMaterial(dMat);
		mesh->SetMaterial(id);
		IDs.push_back(id);
	}

	for (auto& mesh : panningmeshes)
	{
		auto dMat = new DiffuseMaterial_Shadow_Panning();
		dMat->SetDirection(DirectX::XMFLOAT2(0, -1));
		dMat->SetSpeed(0.5f);
		dMat->SetDiffuseTexture(mesh->GetMeshTexture());
		dMat->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
		id = gameContext.pMaterialManager->AddMaterial(dMat);
		mesh->SetMaterial(id);
		IDs.push_back(id);
	}

	m_PlayerSpawn = m_LevelLoader.GetStartPosition();


	auto skinnedDiffuseMaterial = new SkinnedDiffuseMaterial_Shadow();
	skinnedDiffuseMaterial->SetLightDirection(gameContext.pShadowMapper->GetLightDirection());
	skinnedDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/chara_texture.png");
	id = gameContext.pMaterialManager->AddMaterial(skinnedDiffuseMaterial);
	IDs.push_back(id);

	//character
	m_pCharacter = new Character(L"./Resources/Meshes/chara.ovm", id, m_PlayerSpawn, 0.35f, 1.1f, 12.0f, 50.f, 9000.f,0.7f);
	AddChild(m_pCharacter);


	SoundManager::GetInstance()->GetSystem()->createStream("./Resources/Sound/lum.wav", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pLumSFX);
	auto lums = m_LevelLoader.GetLums();
	m_LumCount = lums.size();

	for (auto& lumpos : lums)
	{
		auto lum = new Lum(lumpos);

		auto collectCallback = [actor = (GameObject*)m_pCharacter, LumCount = &m_LumsCollected, scene = this, lum = lum, sfx = m_pLumSFX](GameObject*, GameObject* otherobject, GameObject::TriggerAction)
		{
			if (otherobject == actor && lum->IsActive())
			{
				(*LumCount)++;
				lum->SetActive(false);
				FMOD::Channel* pChannel = nullptr;
				SoundManager::GetInstance()->GetSystem()->getChannel(5, &pChannel);
				SoundManager::GetInstance()->GetSystem()->playSound(sfx, 0, false, &pChannel);
			}
		};
		lum->SetCallback(collectCallback);
		AddChild(lum);
		m_pLums.push_back(lum);
	}

	auto climbing = m_LevelLoader.GetClimbingAreas();
	for (auto& climbs : climbing)
	{
		auto climb = new ClimbArea(climbs.first, climbs.second.y, climbs.second.x, m_pCharacter);
		AddChild(climb);
	}

	auto endposition = m_LevelLoader.GetEndPosition();
	auto end = new EndPortal(endposition.first, endposition.second, m_pCharacter);
	AddChild(end);

	gameContext.pInput->AddInputAction(InputAction(15, Pressed, VK_ESCAPE));


	m_pPPMat = new PostGrain(PostGrain::Time,0.05f,2000.f);

	AddPostProcessingEffect(m_pPPMat);


	SoundManager::GetInstance()->GetSystem()->createStream("./Resources/Sound/The Woods of Light.wav", FMOD_LOOP_NORMAL | FMOD_2D, 0, &m_pMusic);
}

static bool once = true;
void RaymanGame::Update()
{
	const auto gameContext = GetGameContext();
	if (gameContext.pInput->IsActionTriggered(15))
	{
		SceneManager::GetInstance()->SetActiveGameScene(L"ExitMenu");
		StopMusic();
	}

}

void RaymanGame::Draw()
{
	TextRenderer::GetInstance()->DrawText(m_pFont, L"HP   10/10", DirectX::XMFLOAT2(10, 15), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::DarkRed));


	const auto window = OverlordGame::GetGameSettings().Window;

	TextRenderer::GetInstance()->DrawText(m_pFont, std::to_wstring(m_LumsCollected) + L"/" + std::to_wstring(m_LumCount) , DirectX::XMFLOAT2(window.Width - 100.f, 15), static_cast<DirectX::XMFLOAT4>(DirectX::Colors::LightBlue));
}

void RaymanGame::Reset()
{
	m_MaxHP = MAXHP;
	m_HP = m_MaxHP;
	m_CageCount = m_LumsCollected = m_CageCollected = 0;
	m_LumCount = m_pLums.size();
	m_pCharacter->SetPosition(m_PlayerSpawn);

	for (auto lum : m_pLums)
	{
		lum->SetActive(true);
	}
}


void RaymanGame::StartMusic()
{
	if (m_pMusic)
	{
		FMOD::Channel* pChannel = nullptr;
		SoundManager::GetInstance()->GetSystem()->getChannel(1, &pChannel);
		SoundManager::GetInstance()->GetSystem()->playSound(m_pMusic, 0, false, &pChannel);
	}
}

void RaymanGame::StopMusic()
{
	if (m_pMusic)
	{
		FMOD::Channel* pChannel = nullptr;
		SoundManager::GetInstance()->GetSystem()->getChannel(1, &pChannel);
		SoundManager::GetInstance()->GetSystem()->playSound(m_pMusic, 0, true, &pChannel);
	}
}