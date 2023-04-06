// stdafx.cpp : fichier source incluant simplement les fichiers Include standard
// PetitMoteur3D.pch représente l'en-tête précompilé
// stdafx.obj contient les informations de type précompilées

#include "stdafx.h"

using namespace DirectX;
using namespace std;
using namespace physx;

XMVECTOR ToXMVector(const PxVec3& vec) noexcept
{
	return XMVectorSet(vec.x, vec.y, vec.z, 0);
}

XMVECTOR ToXMVector(const XMFLOAT3& vec) noexcept
{
	return XMVectorSet(vec.x, vec.y, vec.z, 0);
}

XMFLOAT3 ToXMFloat3(const PxVec3& vec) noexcept
{
	return XMFLOAT3{ vec.x, vec.y, vec.z };
}

XMFLOAT3 ToXMFloat3(const DirectX::XMVECTOR& vec) noexcept
{
	return XMFLOAT3{ XMVectorGetX(vec), XMVectorGetY(vec), XMVectorGetZ(vec) };
}

PxVec3 ToPxVec3(const XMVECTOR& vec)
{
	return PxVec3{ XMVectorGetX(vec), XMVectorGetY(vec), XMVectorGetZ(vec) };
}

PxVec3 ToPxVec3(const XMFLOAT3& vec)
{
	return PxVec3{ vec.x, vec.y, vec.z };
}

XMVECTOR ToXMQuaternion(const PxQuat& quat) noexcept
{
	return XMVectorSet(quat.x, quat.y, quat.z, quat.w);
}

PxQuat ToPxQuat(const XMVECTOR& quat)
{
	return PxQuat{ XMVectorGetX(quat), XMVectorGetY(quat), XMVectorGetZ(quat), XMVectorGetW(quat) };
}
