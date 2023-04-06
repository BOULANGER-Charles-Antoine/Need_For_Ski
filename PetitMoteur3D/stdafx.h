// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclure les en-têtes Windows rarement utilisés
// Fichiers d'en-tête Windows :
#include <windows.h>

// Fichiers d'en-tête C RunTime
#include <cassert>
#include <cstdint>
#include <tchar.h>

// Fichiers d'en-tête C++ RunTime
#include <exception>
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

// Includes
#define _XM_NO_INTRINSICS_
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dinput.h>

#include "PxPhysicsAPI.h"


//Déclarations
DirectX::XMVECTOR ToXMVector(const physx::PxVec3&) noexcept;
DirectX::XMVECTOR ToXMVector(const DirectX::XMFLOAT3&) noexcept;
DirectX::XMFLOAT3 ToXMFloat3(const physx::PxVec3&) noexcept;
DirectX::XMFLOAT3 ToXMFloat3(const DirectX::XMVECTOR&) noexcept;
physx::PxVec3 ToPxVec3(const DirectX::XMVECTOR&);
physx::PxVec3 ToPxVec3(const DirectX::XMFLOAT3&);
DirectX::XMVECTOR ToXMQuaternion(const physx::PxQuat&) noexcept;
physx::PxQuat ToPxQuat(const DirectX::XMVECTOR&);

constexpr float ToRad(float angleDeg) noexcept { return angleDeg * DirectX::XM_PI / 180; }
constexpr float ToDeg(float angleRad) noexcept { return angleRad * 180 / DirectX::XM_PI; }
