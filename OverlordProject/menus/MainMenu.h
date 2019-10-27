#pragma once
#include "GameScene.h"
#include "SpriteFont.h"

const int confirm = 17;

class MainMenu : public GameScene
{
public:
	MainMenu();
	virtual ~MainMenu();


	MainMenu(const MainMenu& other) = delete;
	MainMenu(MainMenu&& other) noexcept = delete;
	MainMenu& operator=(const MainMenu& other) = delete;
	MainMenu& operator=(MainMenu&& other) noexcept = delete;

	virtual void StartMusic() override;
	virtual void StopMusic() override;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	FMOD::Sound * m_pMusic = nullptr;
	GameObject* m_pBackground;
	SpriteFont* m_pFont;
	int m_menuIndex;
	DirectX::XMFLOAT4 m_Blue, m_Red;
};

