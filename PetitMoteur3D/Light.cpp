#include "stdafx.h"
#include "Light.hpp"

using namespace DirectX;

CLight::CLight() noexcept
{
	ambient = XMVectorSet(0, 0, 0, 0);
	diffuse = XMVectorSet(0, 0, 0, 0);
	specular = XMVectorSet(0, 0, 0, 0);
}

CLight::CLight(XMVECTOR position_in, XMVECTOR _ambient, XMVECTOR _diffuse, XMVECTOR _specular) noexcept :
	ambient { _ambient },
	diffuse { _diffuse },
	specular { _specular }
{
	transform.SetPosition(position_in);
}

CTransform& CLight::GetTransform() noexcept
{
	return transform;
}
CTransform CLight::GetTransform() const noexcept
{
	return transform;
}
DirectX::XMMATRIX CLight::GetMatView() const noexcept
{
	DirectX::XMMATRIX matView = XMMatrixLookAtLH(
		transform.GetPositionVector(),
		transform.GetPositionVector() + transform.GetForward(),
		transform.GetUp()
	);

	return matView;
}

DirectX::XMMATRIX CLight::GetMatProj() const noexcept
{
	DirectX::XMMATRIX matProj = DirectX::XMMatrixPerspectiveFovLH(champDeVision, ratioDAspect, planRapproche, planEloigne);

	return matProj;
}

DirectX::XMMATRIX CLight::GetMatViewProj() const  noexcept
{
	DirectX::XMMATRIX matViewProj = GetMatView() * GetMatProj();

	return matViewProj;
}
const DirectX::XMVECTOR& CLight::GetAmbient() const noexcept
{
	return ambient;
}
const DirectX::XMVECTOR& CLight::GetDiffuse() const noexcept
{
	return diffuse;
}
const DirectX::XMVECTOR& CLight::GetSpecular() const noexcept
{
	return specular;
}

DirectX::XMVECTOR CLight::GetLightDirection(const CTransform& point) const noexcept
{
	return DirectX::XMVECTOR();
}

CPointLight::CPointLight(XMVECTOR position_in, XMVECTOR _ambient, XMVECTOR _diffuse, XMVECTOR _specular) noexcept
	: CLight(position_in, _ambient, _diffuse, _specular)
{
	//Vide
}

DirectX::XMVECTOR CPointLight::GetLightDirection(const CTransform& point) const noexcept
{
	return transform.GetPositionVector() - point.GetPositionVector();
}

CDirectionnalLight::CDirectionnalLight(XMVECTOR position_in, XMVECTOR direction_in, 
	XMVECTOR _ambient, XMVECTOR _diffuse, XMVECTOR _specular) noexcept
	: CLight(position_in, _ambient, _diffuse, _specular)
{
	transform.LookTo(direction_in);
}

DirectX::XMVECTOR CDirectionnalLight::GetLightDirection(const CTransform& point) const noexcept
{
	return -transform.GetForward();
}
