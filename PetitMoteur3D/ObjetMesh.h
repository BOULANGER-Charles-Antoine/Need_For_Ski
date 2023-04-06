#pragma once
#include "Objet3d.h" 
#include "d3dx11effect.h" 
#include "Chargeur.h"
//#include "Material.h"
#include <vector>
#include <map>

namespace PM3D
{

	/*class CSommetMesh
	{
	public:
		CSommetMesh() {};
		CSommetMesh(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _normal, DirectX::XMFLOAT2 _coordTex = DirectX::XMFLOAT2(0, 0));

	public:
		static UINT numElements;
		static D3D11_INPUT_ELEMENT_DESC layout[];

		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 coordTex;
	};*/

	class CObjetMesh : public CObjet3D
	{
		//TEST
	public:
		struct CSommetMesh
		{
			CSommetMesh() = default;
			CSommetMesh(const DirectX::XMFLOAT3& _position, const  DirectX::XMFLOAT3& _normal, const  DirectX::XMFLOAT2& _coordTex = DirectX::XMFLOAT2(0, 0))
				: position(_position)
				, normal(_normal)
				, coordTex(_coordTex)
			{
			}

			static UINT numElements;
			static D3D11_INPUT_ELEMENT_DESC layout[];

			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 coordTex;
		};

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

		struct CMaterial
		{
			std::string NomFichierTexture;
			std::string NomMateriau;
			ID3D11ShaderResourceView* pTextureD3D;

			DirectX::XMFLOAT4 Ambient;
			DirectX::XMFLOAT4 Diffuse;
			DirectX::XMFLOAT4 Specular;
			float Puissance;
			bool transparent;

			CMaterial()
				: pTextureD3D(nullptr)
				, Ambient(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
				, Diffuse(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
				, Specular(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
				, Puissance(1.0f)
				, transparent(false)
			{
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
	public:
		//CObjetMesh(IChargeur& chargeur);
		CObjetMesh(const IChargeur& chargeur);
		CObjetMesh(const IChargeur& chargeur, const std::string& nomfichier);
		CObjetMesh(const std::string& nomfichier);

		virtual ~CObjetMesh();

		void Draw(const CCamera& camera, const CLight& light, const CTransform& transform) override;

	private:

		//----------------AVANT LA MERDE TOTALE -*---------------
		struct ShadersParams
		{
			DirectX::XMMATRIX matWorldViewProj;		// la matrice totale 
			DirectX::XMMATRIX matWorldViewProjLight; // WVP pour lumiere
			DirectX::XMMATRIX matWorld;				// matrice de transformation dans le monde 
			DirectX::XMVECTOR vDirLum;				// la direction de la lumière
			DirectX::XMVECTOR vCamera;				// la position de la cam�ra 
			DirectX::XMVECTOR vAEcl;					// la valeur ambiante de l��clairage 
			DirectX::XMVECTOR vAMat;					// la valeur ambiante du mat�riau 
			DirectX::XMVECTOR vDEcl;					// la valeur diffuse de l��clairage 
			DirectX::XMVECTOR vDMat;					// la valeur diffuse du mat�riau
			DirectX::XMVECTOR vSEcl;					// la valeur sp�culaire de la source d'�clairage
			DirectX::XMVECTOR vSMat;					// la valeur sp�culaire du mat�riau 
			float puissance;				// la puissance de sp�cularit�	
			int bTex;						// Texture ou materiau 
			DirectX::XMFLOAT2 remplissage;
		};

	//	// Pour le dessin 
	//	ID3D11Buffer* pVertexBuffer; 
	//	ID3D11Buffer* pIndexBuffer;

	//	// Les sous-objets 
	//	int NombreSubmesh;					// Nombre de sous-objets dans le mesh 
	//	std::vector<int> SubmeshMaterialIndex;	// Index des mat�riaux 
	//	std::vector<int> SubmeshIndex;			// Index des sous-objets

	//	std::vector<CMaterial> Material;			// Vecteur des mat�riaux

	//	// Pour les effets et shaders 
	//	ID3D11SamplerState* pSampleState; 
	//	ID3D11Buffer* pConstantBuffer; 
	//	ID3DX11Effect* pEffet; 
	//	ID3DX11EffectTechnique* pTechnique; 
	//	ID3DX11EffectPass* pPasse; 
	//	ID3D11InputLayout* pVertexLayout;

	//	CObjetMesh();
	//	void TransfertObjet(IChargeur& chargeur);
	//	void InitEffet();

	//	void EcrireFichierBinaire(const IChargeur& chargeur, const std::string& nomFichier);
	//	void LireFichierBinaire(const std::string& nomFichier);

	//	// Pour les ombres
	//	ID3D11InputLayout* pVertexLayoutShadow;
	//	ID3D11Texture2D* pDepthTexture; // texture de profondeur
	//	ID3D11DepthStencilView* pDepthStencilView;
	//	ID3D11ShaderResourceView* pDepthShaderResourceView;

	//	void InitDepthBuffer();
	//	void InitMatricesShadowMap();

	//protected:
	//	static const int SHADOWMAP_DIM = 512;

	//	DirectX::XMMATRIX mVLight;
	//	DirectX::XMMATRIX mPLight;
	//	DirectX::XMMATRIX mVPLight;
	//};
	
		//---------------------------------------AVANT DESTRUCTION----

		void TransfertObjet(const IChargeur& chargeur);
		void InitEffet();
		void EcrireFichierBinaire(const IChargeur& chargeur, const std::string& nomFichier);
		void LireFichierBinaire(const std::string& nomFichier);
		void InitDepthBuffer();
		//void InitMatricesShadowMap();

		std::unique_ptr<CSommetMesh[]> ts;
		std::vector<int> index;

		//Pour le dessin
		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		// Les sous-objets
		int NombreSubmesh;				// Nombre de sous-objets dans le mesh
		std::vector<int> SubmeshMaterialIndex;// Index des matériaux
		std::vector<int> SubmeshIndex;		// Index des sous-objets

		std::vector<CMaterial> Material;		// Vecteur des matériaux

		// Pour les effets et shaders
		ID3D11SamplerState* pSampleState;
		ID3D11Buffer* pConstantBuffer;
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;

		static const int SHADOWMAP_DIM = 512;

		ID3D11InputLayout* pVertexLayoutShadow;
		ID3D11Texture2D* pTextureShadowMap;         // Texture pour le shadow map
		ID3D11RenderTargetView* pRenderTargetView;  // Vue cible de rendu
		ID3D11ShaderResourceView* pShadowMapView;   // Vue ressource de shader
		ID3D11Texture2D* pDepthTexture;				// texture de profondeur
		ID3D11DepthStencilView* pDepthStencilView;  // Vue tampon de profondeur
		/*
		DirectX::XMMATRIX mVLight;
		DirectX::XMMATRIX mPLight;
		DirectX::XMMATRIX mVPLight;
		*/
	};

	class CGroupMeshes : public CObjet3D {
	public:
		CGroupMeshes(CObjetMesh* obj1);
		CGroupMeshes(CObjetMesh* obj1, CObjetMesh* obj2);
		CGroupMeshes(CObjetMesh* obj1, CObjetMesh* obj2, CObjetMesh* obj3);
		CGroupMeshes(CObjetMesh* obj1, CObjetMesh* obj2, CObjetMesh* obj3, CObjetMesh* obj4);
		~CGroupMeshes();

		//void addMesh(int level, CObjetMesh mesh);
		std::map<int, CObjetMesh*> getMeshes();
		CObjetMesh* getMesh(int level);
		
		int getNbOfMeshes();
		
		void Draw(const CCamera& camera, const CLight& light, const CTransform& transform) override;

	private:
		static constexpr float LEVEL_1_MAX = 50.0f;
		static constexpr float LEVEL_2_MAX = 150.0f;
		static constexpr float LEVEL_3_MAX = 350.0f;

		std::map<int, CObjetMesh*> meshes;

		int nbMeshes;
	};
	
}
