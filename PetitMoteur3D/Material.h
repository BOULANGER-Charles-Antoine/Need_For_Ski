#pragma once
#include <string>


namespace PM3D
{
	struct MaterialBlock
	{
		char NomFichierTexture[200];
		char NomMateriau[60];
		DirectX::XMFLOAT4 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Specular;
		float Puissance;
		bool transparent;
	};
	class CMaterial
	{

	public:
		std::string NomFichierTexture;
		std::string NomMateriau;
		ID3D11ShaderResourceView* pTextureD3D;
		DirectX::XMFLOAT4 Ambient;
		DirectX::XMFLOAT4 Diffuse;
		DirectX::XMFLOAT4 Specular;
		float Puissance;
		bool transparent;

		CMaterial()
		{
			Ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			Diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			Specular = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			transparent = false;
			Puissance = 0;
			pTextureD3D = NULL;
			NomFichierTexture = "";
		}
		void MatToBlock(MaterialBlock& mb)
		{
			strcpy_s(mb.NomFichierTexture, NomFichierTexture.c_str());
			strcpy_s(mb.NomMateriau, NomMateriau.c_str());
			mb.Ambient = Ambient;
			mb.Diffuse = Diffuse;
			mb.Specular = Specular;
			mb.Puissance = Puissance;
			mb.transparent = transparent;

		}

		void BlockToMat(MaterialBlock& mb)
		{
			NomFichierTexture.append(mb.NomFichierTexture);
			NomMateriau.append(mb.NomMateriau);
			Ambient = mb.Ambient;
			Diffuse = mb.Diffuse;
			Specular = mb.Specular;
			Puissance = mb.Puissance;
			transparent = mb.transparent;
		}
	};
}

