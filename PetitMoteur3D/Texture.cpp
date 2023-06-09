#include "StdAfx.h"
#include "strsafe.h"
#include "Texture.h"
#include "resource.h"
#include "util.h"
#include "DDSTextureLoader.h"

using namespace DirectX;

namespace PM3D
{
	CTexture::CTexture()
	{
	}
	CTexture::~CTexture()
	{
		DXRelacher(m_Texture);
	}

	CTexture::CTexture(const std::wstring& filename, CDispositifD3D11*
		pDispositif)
		: m_Filename(filename)
		, m_Texture(nullptr)
	{
		ID3D11Device* pDevice = pDispositif->GetD3DDevice();
		// Charger la texture en ressource
		DXEssayer(CreateDDSTextureFromFile(pDevice,
			m_Filename.c_str(),
			nullptr,
			&m_Texture), DXE_FICHIERTEXTUREINTROUVABLE);
	}
}