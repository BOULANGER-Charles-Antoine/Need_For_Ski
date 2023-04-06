#pragma once
#include "d3dx11effect.h"
#include "Objet3D.h"
#include "DispositifD3D11.h"
#include <map>
#include <string>



namespace PM3D
{

	//class CDispositifD3D11;

	class CSommetSprite
	{
	public:
		CSommetSprite() = default;
		CSommetSprite(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& coordTex)
			: m_Position(position)
			, m_CoordTex(coordTex)
		{
		}

	public:
		static UINT numElements;
		static D3D11_INPUT_ELEMENT_DESC layout[];

		DirectX::XMFLOAT3 m_Position;
		DirectX::XMFLOAT2 m_CoordTex;
	};

	class CAfficheurSprite : public CObjet3D
	{
	public:
		CAfficheurSprite();
		virtual ~CAfficheurSprite();
		virtual void Draw(const CCamera& camera, const CLight& light, const CTransform& transform) override;

		void AjouterSprite(const std::string& NomTexture, const std::string& nom, int _x, int _y, int _dx = 0, int _dy = 0);
		void AjouterPanneau(const std::string& NomTexture, const DirectX::XMFLOAT3& _position,
			float _dx = 0.0f, float _dy = 0.0f);
		void AjouterSpriteTexte(ID3D11ShaderResourceView* pTexture, int _x, int _y);
		std::string getMenuChoice(POINT positionSouris);

		std::map<std::string, std::vector<int>> getObjects();
		std::vector<int> getObject(std::string s);
	private:
		class CSprite
		{
		public:
			ID3D11ShaderResourceView* pTextureD3D;

			DirectX::XMMATRIX matPosDim;
			bool bPanneau;
			CSprite()
				: bPanneau(false)
				, pTextureD3D(nullptr)
			{
			}
		};

		class CPanneau : public CSprite
		{
		public:
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT2 dimension;

			CPanneau()
			{
				bPanneau = true;
			}
		};
	protected:
		static CSommetSprite sommets[6];
		ID3D11Buffer* pVertexBuffer;
		PM3D::CDispositifD3D11* pDispositif;

		ID3D11Buffer* pConstantBuffer;
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;

		ID3D11SamplerState* pSampleState;
	private:
		// Tous nos sprites
		std::vector<std::unique_ptr<CSprite>> tabSprites;

		void InitEffet();

		std::map<std::string, std::vector<int>> objects;
	};

	/*class UIPlayContainer {
	public:
		UIPlayContainer() { afficheur = new CAfficheurSprite(); };
		~UIPlayContainer() = default;

		void update();

		void Draw(const CCamera& camera, const CLight& light, const CTransform& transform);

		CAfficheurSprite* getAfficheur() { return afficheur; }

	private:
		CAfficheurSprite* afficheur;
	};


	class UIPauseContainer {
	public:
		UIPauseContainer() { afficheur = new CAfficheurSprite(); };
		~UIPauseContainer() = default;

		void update();

		void Draw(const CCamera& camera, const CLight& light, const CTransform& transform);

		CAfficheurSprite* getAfficheur() { return afficheur; }

	private:
		CAfficheurSprite* afficheur;
	};*/

} // namespace PM3D