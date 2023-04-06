#pragma once
#include <string>
#include "dispositifD3D11.h"

namespace PM3D
{
	class CTexture
	{
	public:
		CTexture();
		CTexture(const std::wstring& filename, CDispositifD3D11* pDispositif);
		~CTexture();

		const std::wstring& GetFilename() const { return m_Filename; }
		ID3D11ShaderResourceView* GetD3DTexture() { return m_Texture; }

	private:
		std::wstring m_Filename;
		ID3D11ShaderResourceView* m_Texture;
	};
}
