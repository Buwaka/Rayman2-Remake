#include "stdafx.h"
#include "MainMenu.h"
#include "GameObject.h"
#include "SpriteComponent.h"
#include "ContentManager.h"
#include "Components.h"
#include "TextRenderer.h"
#include "SceneManager.h"
#include "OverlordGame.h"
#include "SoundManager.h"


MainMenu::MainMenu()
	:GameScene{ L"MainMenu" }
{
	m_menuIndex = 0;
}

MainMenu::~MainMenu()
{
	m_pMusic->release();
	m_pMusic->release();
}



void MainMenu::Initialize()
{
	auto gameContext = GetGameContext();

	//load background
	m_pBackground = new GameObject();
	m_pBackground->AddComponent(new SpriteComponent(L"./Resources/Textures/menu.png"));
	m_pBackground->GetTransform()->Translate(0.0f, 0.0f, 0.96f);
	AddChild(m_pBackground);

	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Rayman2font.fnt");
	m_Blue = DirectX::XMFLOAT4(DirectX::Colors::LightBlue);
	m_Red = DirectX::XMFLOAT4(DirectX::Colors::LightCoral);

	gameContext.pInput->AddInputAction(InputAction{ confirm,InputTriggerState::Pressed,-1,VK_LBUTTON });


	SoundManager::GetInstance()->GetSystem()->createStream("./Resources/Sound/Menu.wav", FMOD_LOOP_NORMAL | FMOD_2D, 0, &m_pMusic);

	StartMusic();
}

void MainMenu::Update()
{
	auto gameContext = GetGameContext();
	float xPos = float(gameContext.pInput->GetMousePosition().x);
	float yPos = float(gameContext.pInput->GetMousePosition().y);


	if (xPos >= 490 && xPos <= 770)
	{
		if (yPos <= 480 && yPos >= 400)
		{
			m_menuIndex = 1;
			if (gameContext.pInput->IsActionTriggered(confirm))
			{
				StopMusic();
				SceneManager::GetInstance()->SetActiveGameScene(L"RaymanGame");
				SceneManager::GetInstance()->GetGameScene(L"RaymanGame")->StartMusic();
			}
		}
		else if (yPos <= 580 && yPos >= 500)
		{
			m_menuIndex = 2;
			if (gameContext.pInput->IsActionTriggered(confirm))
			{
				PostQuitMessage(0);
			}
		}
	}
	else
		m_menuIndex = 0;
}

void MainMenu::Draw()
{
	switch (m_menuIndex)
	{
	case 1:
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Play", DirectX::XMFLOAT2(575, 400), m_Red);
		break;
	case 2:
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Exit", DirectX::XMFLOAT2(575, 500), m_Red);
		break;
	default:
		break;
	}

	TextRenderer::GetInstance()->DrawText(m_pFont, L"Play", DirectX::XMFLOAT2(575, 400), m_Blue);
	TextRenderer::GetInstance()->DrawText(m_pFont, L"Exit", DirectX::XMFLOAT2(575, 500), m_Blue);
}


void MainMenu::StartMusic()
{
	if (m_pMusic)
	{
		FMOD::Channel* pChannel = nullptr;
		SoundManager::GetInstance()->GetSystem()->getChannel(0, &pChannel);
		SoundManager::GetInstance()->GetSystem()->playSound(m_pMusic, 0, false, &pChannel);
	}
}

void MainMenu::StopMusic()
{
	if (m_pMusic)
	{
		FMOD::Channel* pChannel = nullptr;
		SoundManager::GetInstance()->GetSystem()->getChannel(0, &pChannel);
		SoundManager::GetInstance()->GetSystem()->playSound(m_pMusic, 0, true, &pChannel);
	}
}