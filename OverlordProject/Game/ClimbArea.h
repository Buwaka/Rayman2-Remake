#pragma once
#include "GameObject.h"


class Character;
class ColliderComponent;
class ClimbArea : public GameObject
{
public:
	ClimbArea(DirectX::XMFLOAT3 position, float height, float width, Character* chara);
	~ClimbArea();

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext&) override {};

private:

	ColliderComponent* m_pTrigger;
	DirectX::XMFLOAT3 m_Position;
	float m_Height, m_Width;
	Character* m_pCharacter;
};

