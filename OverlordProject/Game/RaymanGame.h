#pragma once
#include "GameScene.h"
#include "..\Level Loader\LevelLoader.h"
class SpriteFont;
class Character;
class Lum;
class PostProcessingMaterial;

class RaymanGame final : public GameScene
{
public:
	RaymanGame();
	virtual ~RaymanGame();

	RaymanGame(const RaymanGame& other) = delete;
	RaymanGame(RaymanGame&& other) noexcept = delete;
	RaymanGame& operator=(const RaymanGame& other) = delete;
	RaymanGame& operator=(RaymanGame&& other) noexcept = delete;

	void Reset() override;
	virtual void StartMusic() override;
	virtual void StopMusic() override;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	FMOD::Sound * m_pLumSFX;
	FMOD::Sound * m_pMusic = nullptr;

	LevelLoader m_LevelLoader;
	SpriteFont* m_pFont;
	GameObject* m_pLevel;
	PostProcessingMaterial *m_pPPMat;
	std::vector<Lum*> m_pLums;
	std::vector<UINT> IDs;
	DirectX::XMFLOAT3 m_PlayerSpawn;
	Character* m_pCharacter;



	float m_HP, m_MaxHP;
	UINT m_LumCount, m_CageCount, m_LumsCollected, m_CageCollected;
};

