#include "stdafx.h"
#include "Objet3D.h"
#include "MoteurWindows.h"

namespace PM3D
{
	CObjet3D::CObjet3D() noexcept : pDispositif{ CMoteurWindows::GetInstance().GetDispositif() }
	{
		//Vide
	}
	CTexture* CObjet3D::LoadTexture(const std::wstring& texturePath) noexcept
	{
		CGestionnaireDeTextures& texturesManager = CMoteurWindows::GetInstance().GetTextureManager();
		return texturesManager.GetNewTexture(texturePath, pDispositif);
	}
}


