#pragma once
#include "Camera.h"
#include "Light.hpp"
#include "DispositifD3D11.h"
#include "Texture.h"
#include "Material.h"
#include <string>

namespace PM3D
{
	//  Classe : CObjet3D
	//
	//  BUT : 	Classe de base de tous nos objets 3D
	//
	class CObjet3D
	{
	public:
		CObjet3D() noexcept;
		virtual ~CObjet3D() {}

		virtual void Anime(CTransform& transform) {}
		virtual void Draw(const CCamera& camera, const CLight& light, const CTransform& transform) = 0;

	protected:
		PM3D::CDispositifD3D11* pDispositif;

		CTexture* LoadTexture(const std::wstring& texturePath) noexcept;
	};
}
