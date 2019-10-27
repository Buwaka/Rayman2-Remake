#pragma once
#include "Material.h"

class TextureData;

class UberMaterial final : public Material
{
public:
	UberMaterial();
	~UberMaterial();

	//LIGHT
	//*****
	void SetLightDirection(DirectX::XMFLOAT3 direction);

	//DIFFUSE
	//*******
	void EnableDiffuseTexture(bool enable);
	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetDiffuseColor(DirectX::XMFLOAT4 color);

	//SPECULAR
	//********
	void SetSpecularColor(DirectX::XMFLOAT4 color);
	void EnableSpecularLevelTexture(bool enable);
	void SetSpecularLevelTexture(const std::wstring& assetFile);
	void SetShininess(int shininess);

	//AMBIENT
	//*******
	void SetAmbientColor(DirectX::XMFLOAT4 color);
	void SetAmbientIntensity(float intensity);

	//NORMAL MAPPING
	//**************
	void FlipNormalGreenCHannel(bool flip);
	void EnableNormalMapping(bool enable);
	void SetNormalMapTexture(const std::wstring& assetFile);

	//ENVIRONMENT MAPPING
	//*******************
	void EnableEnvironmentMapping(bool enable);
	void SetEnvironmentCube(const std::wstring& assetFile);
	void SetReflectionStrength(float strength);
	void SetRefractionStrength(float strength);
	void SetRefractionIndex(float index);

	//OPACITY
	//*******
	void SetOpacity(float opacity);
	void EnableOpacityMap(bool enable);
	void SetOpacityTexture(const std::wstring& assetFile);

	//SPECULAR MODELS
	//***************
	void EnableSpecularBlinn(bool enable);
	void EnableSpecularPhong(bool enable);

	//FRESNEL FALLOFF
	//***************
	void EnableFresnelFaloff(bool enable);
	void SetFresnelColor(DirectX::XMFLOAT4 color);
	void SetFresnelPower(float power);
	void SetFresnelMultiplier(float multiplier);
	void SetFresnelHardness(float hardness);

protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);

private:
	//LIGHT
	//*****
	DirectX::XMFLOAT3 m_LightDirection = { 0.577f, 0.577f, 0.577f };
	static ID3DX11EffectVectorVariable* m_pLightDirectionVariable;

	//DIFFUSE
	//*******
	bool m_bDiffuseTexture = false;
	static ID3DX11EffectScalarVariable* m_pUseDiffuseTextureVariable;
	TextureData* m_pDiffuseTexture;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	DirectX::XMFLOAT4 m_ColorDiffuse = { 1,1,1,1 };
	static ID3DX11EffectVectorVariable* m_pDiffuseColorVariable;

	//SPECULAR
	//********
	DirectX::XMFLOAT4 m_ColorSpecular = { 1,1,1,1 };
	static ID3DX11EffectVectorVariable* m_pSpecularColorVariable;
	bool m_bSpecularLevelTexture = false;
	static ID3DX11EffectScalarVariable* m_pUseSpecularLevelTextureVariable;
	TextureData* m_pSpecularLevelTexture;
	static ID3DX11EffectShaderResourceVariable* m_pSpecularLevelSRVvariable;
	int m_Shininess = 15;
	static ID3DX11EffectScalarVariable* m_pShininessVariable;

	//AMBIENT
	//*******
	DirectX::XMFLOAT4 m_ColorAmbient = { 0,0,0,1 };
	static ID3DX11EffectVectorVariable* m_pAmbientColorVariable;
	float m_AmbientIntensity = 0.0f;
	static ID3DX11EffectScalarVariable* m_pAmbientIntensityVariable;

	//NORMAL MAPPING
	//**************
	bool m_bFlipGreenChannel = false;
	static ID3DX11EffectScalarVariable* m_pFlipGreenChannelVariable;
	bool m_bNormalMapping = false;
	static ID3DX11EffectScalarVariable* m_pUseNormalMappingVariable;
	TextureData* m_pNormalMappingTexture;
	static ID3DX11EffectShaderResourceVariable* m_pNormalMappingSRVvariable;

	//ENVIRONMENT MAPPING
	//*******************
	bool m_bEnvironmentMapping = false;
	static ID3DX11EffectScalarVariable* m_pUseEnvironmentMappingVariable;
	TextureData* m_pEnvironmentCube;
	static ID3DX11EffectShaderResourceVariable* m_pEnvironmentSRVvariable;
	float m_ReflectionStrength = 0;
	static ID3DX11EffectScalarVariable* m_pReflectionStrengthVariable;
	float m_RefractionStrength = 0;
	static ID3DX11EffectScalarVariable* m_pRefractionStrengthVariable;
	float m_RefractionIndex = 0;
	static ID3DX11EffectScalarVariable* m_pRefractionIndexVariable;

	//OPACITY
	//***************
	float m_Opacity = 1;
	static ID3DX11EffectScalarVariable* m_pOpacityVariable;
	bool m_bOpacityMap = false;
	static ID3DX11EffectScalarVariable* m_pUseOpacityMapVariable;
	TextureData* m_pOpacityMap;
	static ID3DX11EffectShaderResourceVariable* m_pOpacitySRVvariable;

	//SPECULAR MODELS
	//***************
	bool m_bSpecularBlinn = false;
	static ID3DX11EffectScalarVariable* m_pUseBlinnVariable;
	bool m_bSpecularPhong = true;
	static ID3DX11EffectScalarVariable* m_pUsePhongVariable;

	//FRESNEL FALLOFF
	//***************
	bool m_bFresnelFaloff = false;
	static ID3DX11EffectScalarVariable* m_pUseFresnelFalloffVariable;
	DirectX::XMFLOAT4 m_ColorFresnel = {1, 1, 1, 1};
	static ID3DX11EffectVectorVariable* m_pFresnelColorVariable;
	float m_FresnelPower = 0;
	static ID3DX11EffectScalarVariable* m_pFresnelPowerVariable;
	float m_FresnelMultiplier = 0;
	static ID3DX11EffectScalarVariable* m_pFresnelMultiplierVariable;
	float m_FresnelHardness = 0;
	static ID3DX11EffectScalarVariable* m_pFresnelHardnessVariable;
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	UberMaterial(const UberMaterial &obj);
	UberMaterial& operator=(const UberMaterial& obj);
};

