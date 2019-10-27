#pragma once
#include "BaseComponent.h"

struct ParticleEmitterSettings
{
	ParticleEmitterSettings():
		MinSize(0.1f),
		MaxSize(2.0f),
		MinEnergy(1.0f),
		MaxEnergy(2.0f),
		Velocity(DirectX::XMFLOAT3(0, 0, 0)),
		MaxEmitterRange(10.0f),
		MinEmitterRange(9.0f),
		MinSizeGrow(1.0f),
		MaxSizeGrow(1.0f),
		Color(static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White))
	{
	}

	//The minimum size each particle can be at the time when it is spawned
	float MinSize;
	//The maximum size each particle can be at the time when it is spawned
	float MaxSize;
	//The minimum lifetime of each particle, measured in seconds
	float MinEnergy;
	//The maximum lifetime of each particle, measured in seconds
	float MaxEnergy;
	//The starting speed of particles along X, Y and Z, measured in the object's orientation
	DirectX::XMFLOAT3 Velocity;
	//The maximum radius that the particles are spawned in
	float MaxEmitterRange;
	//The minimum radius that the particles are spawned in
	float MinEmitterRange;
	//The percentual maximum change in size during the particle's lifetime
	float MinSizeGrow;
	//The percentual minimum change in size during the particle's lifetime
	float MaxSizeGrow;
	//The color of a particle
	DirectX::XMFLOAT4 Color;
};

struct ParticleVertex
{
	ParticleVertex(const DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0, 0, 0),
		const DirectX::XMFLOAT4 col = static_cast<DirectX::XMFLOAT4>(DirectX::Colors::White), 
		const float size = 5.0f, const float rotation = 0) :
		Position(pos),
		Color(col),
		Size(size),
		Rotation(rotation)
	{
	}

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
	float Size;
	float Rotation;
};

class Particle;
class TextureData;

class ParticleEmitterComponent : public BaseComponent
{

	ID3D11Buffer* m_pVertexBuffer;
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pDefaultTechnique{};
	ID3DX11EffectMatrixVariable *m_pWvpVariable{}, *m_pViewInverseVariable{};
	ID3DX11EffectShaderResourceVariable* m_pTextureVariable{};

	TextureData* m_pParticleTexture;


	ID3D11InputLayout* m_pInputLayout;
	UINT m_pInputLayoutSize;


	std::vector<Particle*> m_Particles;
	ParticleEmitterSettings m_Settings;
	int m_ParticleCount;
	int m_ActiveParticles;
	float m_LastParticleInit;
	float m_Timer;
	bool m_DeathTimer;
	bool m_Active;
	std::wstring m_AssetFile;


	void LoadEffect(const GameContext& gameContext);
	void CreateVertexBuffer(const GameContext& gameContext);

public:
	ParticleEmitterComponent(const ParticleEmitterComponent& other) = delete;
	ParticleEmitterComponent(ParticleEmitterComponent&& other) noexcept = delete;
	ParticleEmitterComponent& operator=(const ParticleEmitterComponent& other) = delete;
	ParticleEmitterComponent& operator=(ParticleEmitterComponent&& other) noexcept = delete;

	ParticleEmitterComponent(std::wstring assetFile, int particleCount = 50);
	~ParticleEmitterComponent();

	void SetDeathTimer(float time, const GameContext& gameContext);

	void SetMinSize(float minSize) { m_Settings.MinSize = minSize; }
	void SetMaxSize(float maxSize) { m_Settings.MaxSize = maxSize; }
	void SetMinEnergy(float minEnergy) { m_Settings.MinEnergy = minEnergy; }
	void SetMaxEnergy(float maxEnergy) { m_Settings.MaxEnergy = maxEnergy; }
	void SetVelocity(DirectX::XMFLOAT3 velocity) { m_Settings.Velocity = velocity; }
	void SetMinSizeGrow(float minSizeGrow) { m_Settings.MinSizeGrow = minSizeGrow; }
	void SetMaxSizeGrow(float maxSizeGrow) { m_Settings.MaxSizeGrow = maxSizeGrow; }
	void SetMinEmitterRange(float minEmitterRange) { m_Settings.MinEmitterRange = minEmitterRange; }
	void SetMaxEmitterRange(float maxEmitterRange) { m_Settings.MaxEmitterRange = maxEmitterRange; }
	void SetColor(DirectX::XMFLOAT4 color) { m_Settings.Color = color; }
	void SetActive(bool value) { m_Active = value; };

protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& gameContext) override;
	void PostDraw(const GameContext& gameContext) override;
};
