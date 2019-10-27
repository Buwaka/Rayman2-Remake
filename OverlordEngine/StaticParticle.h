#pragma once
#include "BaseComponent.h"



class TextureData;
struct ParticleVertex;

class StaticParticle : public BaseComponent
{
	ID3D11Buffer* m_pVertexBuffer;
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pDefaultTechnique{};
	ID3DX11EffectMatrixVariable *m_pWvpVariable{}, *m_pViewInverseVariable{};
	ID3DX11EffectShaderResourceVariable* m_pTextureVariable{};

	ID3D11InputLayout* m_pInputLayout;
	UINT m_pInputLayoutSize;

	std::wstring m_AssetFile;
	TextureData* m_pParticleTexture;
	ParticleVertex* m_ParticleVertex;
	bool m_Changed;
	bool m_Active;

	//Method to load effect-related stuff
	void LoadEffect(const GameContext& gameContext);
	//Method to create the vertex buffer
	void CreateVertexBuffer(const GameContext& gameContext);

public:
	StaticParticle(std::wstring assetFile, DirectX::XMFLOAT3 Position, DirectX::XMFLOAT4 Color = DirectX::XMFLOAT4(DirectX::Colors::White), float size = 1.0f, float rotation = 0);
	~StaticParticle();


	void ChangeParticle(ParticleVertex* particle) { m_ParticleVertex = particle; m_Changed = true; }
	void SetActive(bool value) { m_Active = value; };
	void SetRotation(float degrees);

	StaticParticle(const StaticParticle& other) = delete;
	StaticParticle(StaticParticle&& other) noexcept = delete;
	StaticParticle& operator=(const StaticParticle& other) = delete;
	StaticParticle& operator=(StaticParticle&& other) noexcept = delete;


protected:
	void Initialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;
	void Draw(const GameContext& ) override {};
	void PostDraw(const GameContext& gameContext) override;
};

