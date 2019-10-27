#pragma once
#include "GameScene.h"
#include "SpriteFont.h"
#include "SoundManager.h"

const int End_confirm = 18;

class EndingMenu : public GameScene
{
public:

	EndingMenu();
	virtual ~EndingMenu();
	EndingMenu(const EndingMenu& other) = delete;
	EndingMenu(EndingMenu&& other) noexcept = delete;
	EndingMenu& operator=(const EndingMenu& other) = delete;
	EndingMenu& operator=(EndingMenu&& other) noexcept = delete;

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
	DirectX::XMFLOAT4 m_Blue, m_Red;
	int m_menuIndex;
};

