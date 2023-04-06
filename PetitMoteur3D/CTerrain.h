#pragma once
#include <vector>
#include <string>

#include "Sommet.h"
#include "d3dx11effect.h"
#include "Texture.h"
#include "Objet3D.h"

namespace PM3D
{
	class CTerrain : public CObjet3D
	{

	public:
		CTerrain(const std::wstring& heightmap);
		virtual ~CTerrain();

		virtual void Draw(const CCamera& camera, const CLight& light, const CTransform& transform) override;

		int GetSizeX() const;
		int GetSizeY() const;
		int GetNbPolygons() const;
		int GetNbSommets() const;
		const std::vector<CSommetBloc>& GetSommets() const;
		const std::vector<unsigned int>& GetIndexes() const;

		void SetForeground(const std::wstring& texturePath);
		void SetBackground(const std::wstring& texturePath);
		void SetGround(const std::wstring& texturePath);
	private:
		void MakeTerrain(const std::wstring& heightmap);
		void InitEffet();

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		// D�finitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;

		// Pour les effets
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;

		std::vector<CSommetBloc> vectSommets;
		std::vector<unsigned int> pIndexesVect;
		int nbPolygons;
		int nbSommets;
		int sizeX;
		int sizeY;

		float temps = 0;		//variable vide
		float textureRepeat = 5.0f;

		ID3D11ShaderResourceView* pForeground;
		ID3D11ShaderResourceView* pBackground;
		ID3D11ShaderResourceView* pGround;
		ID3D11SamplerState* pSampleState;
	};

}
