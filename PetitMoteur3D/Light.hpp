#pragma once
#include "Transform.h"

class CLight 
{
public:
	CLight() noexcept;
	CLight(DirectX::XMVECTOR position_in, DirectX::XMVECTOR _ambient, DirectX::XMVECTOR _diffuse, DirectX::XMVECTOR _specular) noexcept;
	virtual ~CLight() = default;

	CTransform& GetTransform() noexcept;
	CTransform GetTransform() const noexcept;
	DirectX::XMMATRIX GetMatView() const noexcept;
	DirectX::XMMATRIX GetMatProj() const noexcept;
	DirectX::XMMATRIX GetMatViewProj() const noexcept;

	const DirectX::XMVECTOR& GetAmbient() const noexcept;
	const DirectX::XMVECTOR& GetDiffuse() const noexcept;
	const DirectX::XMVECTOR& GetSpecular() const noexcept;

	virtual DirectX::XMVECTOR GetLightDirection(const CTransform& point) const noexcept = 0;

protected:
	CTransform transform{};
	DirectX::XMVECTOR ambient;
	DirectX::XMVECTOR diffuse;
	DirectX::XMVECTOR specular;

	const float champDeVision = DirectX::XM_PI / 4.0f;  // 45 degrés
	const float ratioDAspect = 1.0f; 	// 512/512
	const float planRapproche = 2.0f; 	// Pas besoin d'être trop près
	const float planEloigne = 5000.0f;	// Suffisemment pour avoir tous les objets
};

class CPointLight : public CLight
{
public:
	CPointLight(DirectX::XMVECTOR position_in, DirectX::XMVECTOR _ambient, DirectX::XMVECTOR _diffuse, DirectX::XMVECTOR _specular) noexcept;

	DirectX::XMVECTOR GetLightDirection(const CTransform& point) const noexcept override;
};

class CDirectionnalLight : public CLight
{
public:
	CDirectionnalLight(DirectX::XMVECTOR position_in, DirectX::XMVECTOR direction_in, 
		DirectX::XMVECTOR _ambient, DirectX::XMVECTOR _diffuse, DirectX::XMVECTOR _specular) noexcept;

	DirectX::XMVECTOR GetLightDirection(const CTransform& point) const noexcept override;
};