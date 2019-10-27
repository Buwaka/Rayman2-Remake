#include "stdafx.h"
#include "EndingMenu.h"
#include "GameObject.h"
#include "SpriteComponent.h"
#include "ContentManager.h"
#include "Components.h"
#include "SceneManager.h"
#include "TextRenderer.h"
#include "SoundManager.h"

EndingMenu::EndingMenu()
	:GameScene{ L"EndingMenu" }
{
	m_menuIndex = 0;
}

EndingMenu::~EndingMenu()
{
	m_pMusic->release();
}

void EndingMenu::Initialize()
{
	auto gameContext = GetGameContext();


	m_pBackground = new GameObject();
	m_pBackground->AddComponent(new SpriteComponent(L"./Resources/Textures/endscreen.png"));
	m_pBackground->GetTransform()->Translate(0.0f, 0.0f, 0.95f);
	AddChild(m_pBackground);


	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Rayman2font.fnt");
	m_Blue = DirectX::XMFLOAT4(DirectX::Colors::LightBlue);
	m_Red = DirectX::XMFLOAT4(DirectX::Colors::LightCoral);



	gameContext.pInput->AddInputAction(InputAction{ End_confirm,InputTriggerState::Pressed,-1,VK_LBUTTON });


	SoundManager::GetInstance()->GetSystem()->createStream("./Resources/Sound/End.wav", FMOD_LOOP_OFF | FMOD_2D, 0, &m_pMusic);


};

void EndingMenu::Update()
{
	auto gameContext = GetGameContext();
	float xPos = float(gameContext.pInput->GetMousePosition().x);
	float yPos = float(gameContext.pInput->GetMousePosition().y);


	if (xPos >= 490 && xPos <= 770)
	{
		//if (yPos <= 580 && yPos >= 500)
		//{
		//	m_menuIndex = 2;
		//	if (gameContext.pInput->IsActionTriggered(End_confirm))
		//	{
		//		SceneManager::GetInstance()->GetGameScene(L"RaymanGame")->Reset();
		//		SceneManager::GetInstance()->SetActiveGameScene(L"RaymanGame");
		//		SceneManager::GetInstance()->GetGameScene(L"RaymanGame")->StartMusic();
		//	}
		//}
		if (yPos <= 680 && yPos >= 600)
		{
			m_menuIndex = 3;
			if (gameContext.pInput->IsActionTriggered(End_confirm))
			{
				StopMusic();
				SceneManager::GetInstance()->GetGameScene(L"RaymanGame")->Reset();
				SceneManager::GetInstance()->SetActiveGameScene(L"MainMenu");
				SceneManager::GetInstance()->GetGameScene(L"MainMenu")->StartMusic();
			}
		}
	}
	else
		m_menuIndex = 0;
}

void EndingMenu::Draw()
{
	switch (m_menuIndex)
	{
	//case 2:
	//	TextRenderer::GetInstance()->DrawText(m_pFont, L"Restart", DirectX::XMFLOAT2(530, 500), m_Red);
	//	break;
	case 3:
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Menu", DirectX::XMFLOAT2(575, 600), m_Red);
		break;
	default:
		break;
	}
	//TextRenderer::GetInstance()->DrawText(m_pFont, L"Restart", DirectX::XMFLOAT2(530, 500), m_Blue);
	TextRenderer::GetInstance()->DrawText(m_pFont, L"Menu", DirectX::XMFLOAT2(575, 600), m_Blue);
}


void EndingMenu::StartMusic()
{
	if (m_pMusic)
	{
		FMOD::Channel* pChannel = nullptr;
		SoundManager::GetInstance()->GetSystem()->getChannel(2, &pChannel);
		SoundManager::GetInstance()->GetSystem()->playSound(m_pMusic, 0, false, &pChannel);
	}
}

void EndingMenu::StopMusic()
{
	if (m_pMusic)
	{
		FMOD::Channel* pChannel = nullptr;
		SoundManager::GetInstance()->GetSystem()->getChannel(2, &pChannel);
		SoundManager::GetInstance()->GetSystem()->playSound(m_pMusic, 0, true, &pChannel);
	}
}