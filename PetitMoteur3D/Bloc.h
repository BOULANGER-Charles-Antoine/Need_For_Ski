#pragma once
#include "Objet3D.h"
#include "d3dx11effect.h"

namespace PM3D
{
	const uint16_t index_bloc[36] = {
			0,1,2,			// devant
			0,2,3,			// devant
			5,6,7,			// arrière
			5,7,4,			// arrière
			8,9,10,			// dessous
			8,10,11,		// dessous
			13,14,15,		// dessus
			13,15,12,		// dessus
			19,16,17,		// gauche
			19,17,18,		// gauche
			20,21,22,		// droite
			20,22,23		// droite
	};

	//  Classe : CBloc
	//
	//  BUT : 	Classe de bloc
	//
	class CBloc : public PM3D::CObjet3D
	{
	public:
		CBloc(const float dx, const float dy, const float dz);
		virtual ~CBloc();

		void Draw(const CCamera& camera, const CLight& light, const CTransform& transform) override;

	private:
		void InitShaders();

		// Les buffers
		ID3D11Buffer* pConstantBuffer = nullptr;
		ID3D11Buffer* pVertexBuffer = nullptr;
		ID3D11Buffer* pIndexBuffer = nullptr;

		// Pour les effets
		ID3DX11Effect* pEffet = nullptr;
		ID3DX11EffectTechnique* pTechnique = nullptr;
		ID3DX11EffectPass* pPasse = nullptr;
		ID3D11InputLayout* pVertexLayout = nullptr;
	};
}