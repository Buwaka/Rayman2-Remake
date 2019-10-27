#include "stdafx.h"
#include "ExitMenu.h"
#include "GameObject.h"
#include "SpriteComponent.h"
#include "ContentManager.h"
#include "Components.h"
#include "SceneManager.h"
#include "TextRenderer.h"
#include "SoundManager.h"

ExitMenu::ExitMenu()
	:GameScene{ L"ExitMenu" }
{
	m_menuIndex = 0;
}

void ExitMenu::Initialize()
{
	auto gameContext = GetGameContext();


	m_pBackground = new GameObject();
	m_pBackground->AddComponent(new SpriteComponent(L"./Resources/Textures/menu.png"));
	m_pBackground->GetTransform()->Translate(0.0f, 0.0f, 0.95f);
	AddChild(m_pBackground);


	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/Rayman2font.fnt");
	m_Blue = DirectX::XMFLOAT4(DirectX::Colors::LightBlue);
	m_Red = DirectX::XMFLOAT4(DirectX::Colors::LightCoral);



	gameContext.pInput->AddInputAction(InputAction{ Exit_confirm,InputTriggerState::Pressed,-1,VK_LBUTTON });





};

void ExitMenu::Update()
{
	auto gameContext = GetGameContext();
	float xPos = float(gameContext.pInput->GetMousePosition().x);
	float yPos = float(gameContext.pInput->GetMousePosition().y);


	if (xPos >= 490 && xPos <= 770)
	{
		if (yPos <= 480 && yPos >= 400)
		{
			m_menuIndex = 1;
			if (gameContext.pInput->IsActionTriggered(Exit_confirm))
			{
				SceneManager::GetInstance()->SetActiveGameScene(L"RaymanGame");
				SceneManager::GetInstance()->GetGameScene(L"RaymanGame")->StartMusic();
			}
		}
		else if (yPos <= 580 && yPos >= 500)
		{
			m_menuIndex = 2;
			if (gameContext.pInput->IsActionTriggered(Exit_confirm))
			{
				SceneManager::GetInstance()->GetGameScene(L"RaymanGame")->Reset();
				SceneManager::GetInstance()->SetActiveGameScene(L"RaymanGame");
				SceneManager::GetInstance()->GetGameScene(L"RaymanGame")->StartMusic();
			}
		}
		else if (yPos <= 680 && yPos >= 600)
		{
			m_menuIndex = 3;
			if (gameContext.pInput->IsActionTriggered(Exit_confirm))
			{
				SceneManager::GetInstance()->SetActiveGameScene(L"MainMenu");
				SceneManager::GetInstance()->GetGameScene(L"MainMenu")->StartMusic();
			}
		}
	}
	else
		m_menuIndex = 0;
}

void ExitMenu::Draw()
{
	switch (m_menuIndex)
	{
	case 1:
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Continue", DirectX::XMFLOAT2(515, 400), m_Red);
		break;
	case 2:
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Restart", DirectX::XMFLOAT2(530, 500), m_Red);
		break;
	case 3:
		TextRenderer::GetInstance()->DrawText(m_pFont, L"Exit", DirectX::XMFLOAT2(575, 600), m_Red);
		break;
	default:
		break;
	}

	TextRenderer::GetInstance()->DrawText(m_pFont, L"Continue", DirectX::XMFLOAT2(515, 400), m_Blue);
	TextRenderer::GetInstance()->DrawText(m_pFont, L"Restart", DirectX::XMFLOAT2(530, 500), m_Blue);
	TextRenderer::GetInstance()->DrawText(m_pFont, L"Exit", DirectX::XMFLOAT2(575, 600), m_Blue);
}

