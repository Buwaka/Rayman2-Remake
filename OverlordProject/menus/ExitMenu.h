#pragma once
#include "GameScene.h"
#include "SpriteFont.h"
#include "SoundManager.h"

const int Exit_confirm = 16;

class ExitMenu : public GameScene
{
public:

	ExitMenu();
	virtual ~ExitMenu() = default;
	ExitMenu(const ExitMenu& other) = delete;
	ExitMenu(ExitMenu&& other) noexcept = delete;
	ExitMenu& operator=(const ExitMenu& other) = delete;
	ExitMenu& operator=(ExitMenu&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	GameObject* m_pBackground;
	SpriteFont* m_pFont;
	DirectX::XMFLOAT4 m_Blue, m_Red;
	int m_menuIndex;
};

