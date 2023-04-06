#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>

#include "Sommet.h"
#include "d3dx11effect.h"
#include "Texture.h"
#include "Objet3D.h"

namespace PM3D
{
	class Skybox : public CObjet3D
	{
	public:
		Skybox(const std::wstring& skybox);
		virtual ~Skybox();

		virtual void Draw(const CCamera& camera, const CLight& light, const CTransform& transform) override;

		int GetNbSommets() const;
		const std::vector<CSommetBloc>& GetSommets() const;
		const std::vector<unsigned int>& GetIndexes() const;

		void Load();
		void SetTexture(const std::wstring& texturePath);

	private:
		void MakeSkybox(const std::wstring& skybox);
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
		int nbSommets;

		float textureRepeatX = 2.0f;
		float textureRepeatY = 1.8f;

		ID3D11ShaderResourceView* pTexture;
		ID3D11SamplerState* pSampleState;
	};
}

#endif // !SKYBOX_H