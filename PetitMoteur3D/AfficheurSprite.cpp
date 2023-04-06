#include "StdAfx.h"
#include "AfficheurSprite.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "util.h"
#include "DispositifD3D11.h"
#include "Scene.h"
#include <iomanip>
#include <algorithm>

using namespace DirectX;

namespace PM3D
{
	struct ShadersParams
	{
		XMMATRIX matWVP;	// la matrice totale 
	};

	// Definir l'organisation de notre sommet
	D3D11_INPUT_ELEMENT_DESC CSommetSprite::layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT CSommetSprite::numElements = ARRAYSIZE(layout);

	CSommetSprite CAfficheurSprite::sommets[6] =
	{
		CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
		CSommetSprite(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)),
		CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
		CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
		CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
		CSommetSprite(XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f))
	};

	CAfficheurSprite::CAfficheurSprite() : pDispositif{ CMoteurWindows::GetInstance().GetDispositif() }
	{

		// Cr�ation du vertex buffer et copie des sommets
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(sommets);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = sommets;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer),
			DXE_CREATIONVERTEXBUFFER);

		// Initialisation de l'effet
		InitEffet();
	}

	CAfficheurSprite ::~CAfficheurSprite()
	{
		DXRelacher(pConstantBuffer);
		DXRelacher(pSampleState);

		DXRelacher(pEffet);
		DXRelacher(pVertexLayout);
		DXRelacher(pVertexBuffer);

		tabSprites.clear();
	}

	void CAfficheurSprite::InitEffet()
	{
		// Compilation et chargement du vertex shader
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		// Cr�ation d'un tampon pour les constantes de l'effet
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

		// Pour l'effet
		ID3DBlob* pFXBlob = nullptr;

		DXEssayer(D3DCompileFromFile(L"resources/Sprite.fx", 0, 0, 0,
			"fx_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
			&pFXBlob, 0),
			DXE_ERREURCREATION_FX);

		D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(),
			pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

		pFXBlob->Release();

		pTechnique = pEffet->GetTechniqueByIndex(0);
		pPasse = pTechnique->GetPassByIndex(0);

		// Cr�er l'organisation des sommets pour le VS de notre effet
		D3DX11_PASS_SHADER_DESC effectVSDesc;
		pPasse->GetVertexShaderDesc(&effectVSDesc);

		D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex,
			&effectVSDesc2);

		const void* vsCodePtr = effectVSDesc2.pBytecode;
		const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

		DXEssayer(pD3DDevice->CreateInputLayout(
			CSommetSprite::layout,
			CSommetSprite::numElements,
			vsCodePtr,
			vsCodeLen,
			&pVertexLayout),
			DXE_CREATIONLAYOUT);

		// Initialisation des param�tres de sampling de la texture
		D3D11_SAMPLER_DESC samplerDesc{};

		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 4;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		// Cr�ation de l'�tat de sampling
		pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
	}

	void CAfficheurSprite::Draw(const CCamera& camera, const CLight& light, const CTransform& transform)
	{
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext =
			pDispositif->GetImmediateContext();

		// Choisir la topologie des primitives
		pImmediateContext->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Source des sommets
		const UINT stride = sizeof(CSommetSprite);
		const UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride,
			&offset);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);

		// Le sampler state
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);

		ID3DX11EffectConstantBuffer* pCB =
			pEffet->GetConstantBufferByName("param");
		ID3DX11EffectShaderResourceVariable* variableTexture;
		variableTexture =
			pEffet->GetVariableByName("textureEntree")->AsShaderResource();

		pDispositif->ActiverMelangeAlpha();

		// Faire le rendu de tous nos sprites
		for (auto& sprite : tabSprites)
		{
			// Initialiser et s�lectionner les �constantes� de l'effet
			ShadersParams sp;
			sp.matWVP = XMMatrixTranspose(sprite->matPosDim);
			pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr,
				&sp, 0, 0);

			pCB->SetConstantBuffer(pConstantBuffer);

			// Activation de la texture
			variableTexture->SetResource(sprite->pTextureD3D);

			pPasse->Apply(0, pImmediateContext);

			// **** Rendu de l'objet
			pImmediateContext->Draw(6, 0);
		}

		pDispositif->DesactiverMelangeAlpha();
	}

	void CAfficheurSprite::AjouterSprite(const std::string& NomTexture,
		const std::string& nom,
		int _x, int _y,
		int _dx, int _dy)
	{
		float x, y, dx, dy;
		float posX, posY;
		float facteurX, facteurY;

		objects[nom] = std::vector<int>{ _x, _y, _dx, _dy };

		// Initialisation de la texture
		CGestionnaireDeTextures& TexturesManager =
			CMoteurWindows::GetInstance().GetTextureManager();

		std::wstring ws(NomTexture.begin(), NomTexture.end());

		std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();;
		pSprite->pTextureD3D =
			TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();

		// Obtenir les dimensions de la texture si _dx et _dy sont � 0;
		if (_dx == 0 && _dy == 0)
		{
			ID3D11Resource* pResource;
			ID3D11Texture2D* pTextureInterface = 0;
			pSprite->pTextureD3D->GetResource(&pResource);
			pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
			D3D11_TEXTURE2D_DESC desc;
			pTextureInterface->GetDesc(&desc);

			DXRelacher(pResource);
			DXRelacher(pTextureInterface);

			dx = float(desc.Width);
			dy = float(desc.Height);
		}
		else
		{
			dx = float(_dx);
			dy = float(_dy);
		}

		// Dimension en facteur
		facteurX = dx * 2.0f / pDispositif->GetLargeur();
		facteurY = dy * 2.0f / pDispositif->GetHauteur();

		// Position en coordonn�es logiques
		// 0,0 pixel = -1,1   
		x = float(_x);
		y = float(_y);

		posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
		posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

		pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
			XMMatrixTranslation(posX, posY, 0.0f);

		// On l'ajoute � notre vecteur
		tabSprites.push_back(std::move(pSprite));
	}

	void CAfficheurSprite::AjouterPanneau(const std::string& NomTexture,
		const XMFLOAT3& _position,
		float _dx, float _dy)
	{
		// Initialisation de la texture
		CGestionnaireDeTextures& TexturesManager =
			CMoteurWindows::GetInstance().GetTextureManager();

		std::wstring ws(NomTexture.begin(), NomTexture.end());

		std::unique_ptr<CPanneau> pPanneau = std::make_unique<CPanneau>();
		pPanneau->pTextureD3D =
			TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();

		// Obtenir la dimension de la texture si _dx et _dy sont � 0;
		if (_dx == 0.0f && _dy == 0.0f)
		{
			ID3D11Resource* pResource;
			ID3D11Texture2D* pTextureInterface = 0;
			pPanneau->pTextureD3D->GetResource(&pResource);
			pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
			D3D11_TEXTURE2D_DESC desc;
			pTextureInterface->GetDesc(&desc);

			DXRelacher(pResource);
			DXRelacher(pTextureInterface);

			pPanneau->dimension.x = float(desc.Width);
			pPanneau->dimension.y = float(desc.Height);

			// Dimension en facteur
			pPanneau->dimension.x = pPanneau->dimension.x * 2.0f / pDispositif->GetLargeur();
			pPanneau->dimension.y = pPanneau->dimension.y * 2.0f / pDispositif->GetHauteur();
		}
		else
		{
			pPanneau->dimension.x = float(_dx);
			pPanneau->dimension.y = float(_dy);
		}

		// Position en coordonn�es du monde

		if (CMoteurWindows::GetInstance().GetCurrentScene()->GetCamera().expired()) return;
		auto cam = CMoteurWindows::GetInstance().GetCurrentScene()->GetCamera().lock();
		const XMMATRIX& viewProj = cam.get()->GetMatViewProj();
		pPanneau->position = _position;

		pPanneau->matPosDim = XMMatrixScaling(pPanneau->dimension.x,
			pPanneau->dimension.y, 1.0f) *
			XMMatrixRotationY(DirectX::XM_PI/2)	*
			XMMatrixTranslation(pPanneau->position.x,
				pPanneau->position.y, pPanneau->position.z) *
			viewProj;

		// On l'ajoute � notre vecteur
		tabSprites.push_back(std::move(pPanneau));
	}

	void CAfficheurSprite::AjouterSpriteTexte(
		ID3D11ShaderResourceView* pTexture, int _x, int _y)
	{
		std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();
		pSprite->pTextureD3D = pTexture;

		// Obtenir la dimension de la texture;
		ID3D11Resource* pResource;
		ID3D11Texture2D* pTextureInterface = 0;
		pSprite->pTextureD3D->GetResource(&pResource);
		pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
		D3D11_TEXTURE2D_DESC desc;
		pTextureInterface->GetDesc(&desc);

		DXRelacher(pResource);
		DXRelacher(pTextureInterface);

		const float dx = float(desc.Width);
		const float dy = float(desc.Height);

		// Dimension en facteur
		const float facteurX = dx * 2.0f / pDispositif->GetLargeur();
		const float facteurY = dy * 2.0f / pDispositif->GetHauteur();

		// Position en coordonn�es logiques
		// 0,0 pixel = -1,1   
		const float x = float(_x);
		const float y = float(_y);

		const float posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
		const float posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

		pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
			XMMatrixTranslation(posX, posY, 0.0f);

		// On l'ajoute � notre vecteur
		tabSprites.push_back(std::move(pSprite));
	}


	std::map<std::string, std::vector<int>> CAfficheurSprite::getObjects()
	{
		return objects;
	}

	std::vector<int> PM3D::CAfficheurSprite::getObject(std::string s)
	{
		return objects[s];
	}

	std::string PM3D::CAfficheurSprite::getMenuChoice(POINT positionSouris) {
		auto mapObject = getObjects();
		std::string result{};

		float coeffX = static_cast<float>(PM3D::CMoteurWindows::GetInstance().GetDispositif()->GetLargeur()) / PM3D::CMoteurWindows::GetInstance().GetDispositif()->getResolutionOriginalX();
		float coeffY = static_cast<float>(PM3D::CMoteurWindows::GetInstance().GetDispositif()->GetHauteur()) / PM3D::CMoteurWindows::GetInstance().GetDispositif()->getResolutionOriginalY();

		if (coeffX > 1.0f && coeffY > 1.0f)
			int a{};

		for (auto m : mapObject) {
			if (static_cast<int>(positionSouris.x) >= coeffX * m.second.at(0)                        &&
				static_cast<int>(positionSouris.x) <= coeffX * (m.second.at(0) + m.second.at(2)) &&
				static_cast<int>(positionSouris.y) <= coeffY * m.second.at(1)                        &&
				static_cast<int>(positionSouris.y) >= coeffY * (m.second.at(1) - m.second.at(3))   )
			{
				result = m.first;
				break;
			}
		}

		return result;
	}


	//void PM3D::UIPlayContainer::update()
	//{
	//	if (afficheur) delete(afficheur);
	//	afficheur = new PM3D::CAfficheurSprite();

	//	afficheur->AjouterSprite("resources/IconeVitesse.dds", "IconeVitesse", 870, 740, 40, 40);

	//	afficheur->AjouterSprite("resources/IconeChrono.dds", "IconeChrono", 467, 50, 40, 40);

	//	// Setup de l'afficheurTexte
	//	PM3D::CAfficheurTexte::Init();
	//	const Gdiplus::FontFamily oFamily(L"Arial", nullptr);
	//	Gdiplus::Font* pPolice = new Gdiplus::Font(&oFamily, 18.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

	//	// R�cup�ration de la vitesse du joueur + conversion
	//	auto& a = static_cast<physx::PxRigidDynamic&>(PM3D::CMoteurWindows::GetInstance().GetCurrentScene()->GetPlayer().lock()->GetActor());
	//	std::stringstream stream;
	//	stream << std::fixed << std::setprecision(2) << a.getLinearVelocity().magnitude();
	//	std::string b = stream.str();
	//	std::wstring widestr = std::wstring(b.begin(), b.end());
	//	const wchar_t* widecstr = widestr.c_str();

	//	// recup�ration du chrono + conversion
	//	PM3D::CMoteurWindows::GetInstance().GetCurrentScene()->chrono += Time::GetDeltaTime();
	//	std::stringstream timerstream;
	//	timerstream << std::fixed << std::setprecision(2) << PM3D::CMoteurWindows::GetInstance().GetCurrentScene()->chrono;
	//	std::string timerstr = timerstream.str();
	//	std::wstring timerwstr = std::wstring(timerstr.begin(), timerstr.end());
	//	const wchar_t* timerwcstr = timerwstr.c_str();

	//	// Cr�ation d'un afficheur (on ne peut pas �crire 2 fois sur le m�me)

	//	PM3D::CAfficheurTexte* vitesseVal = new PM3D::CAfficheurTexte(38, 30, pPolice);
	//	vitesseVal->Ecrire(widecstr);


	//	PM3D::CAfficheurTexte* vitesseUnits = new PM3D::CAfficheurTexte(100, 30, pPolice);
	//	vitesseUnits->Ecrire(L"km/h");


	//	PM3D::CAfficheurTexte* timer = new PM3D::CAfficheurTexte(40, 20, pPolice);
	//	timer->Ecrire(timerwcstr);


	//	//afficheur->AjouterSpriteTexte(vitesseTexte->GetTextureView(), 850, 740);
	//	afficheur->AjouterSpriteTexte(vitesseVal->GetTextureView(), 930, 740);

	//	afficheur->AjouterSpriteTexte(vitesseUnits->GetTextureView(), 968, 740);

	//	afficheur->AjouterSpriteTexte(timer->GetTextureView(), 517, 40);
	//}

	//void PM3D::UIPlayContainer::Draw(const CCamera& camera, const CLight& light, const CTransform& transform)
	//{
	//	afficheur->Draw(camera, light, transform);
	//}


	//void UIPauseContainer::update()
	//{
	//	if (afficheur) delete(afficheur);
	//	afficheur = new PM3D::CAfficheurSprite();

	//	afficheur->AjouterSprite("resources/PlayButton.dds", "PlayButton", 375, 300, 255, 160);
	//	afficheur->AjouterSprite("resources/OptionsButton.dds", "OptionsButton", 375, 500, 255, 160);
	//	
	//	//add sprite aux objets pour chaque container
	//}

	//void UIPauseContainer::Draw(const CCamera& camera, const CLight& light, const CTransform& transform)
	//{
	//	afficheur->Draw(camera, light, transform);
	//}

} // namespace PM3D






























//#include "stdafx.h"
//#include "AfficheurSprite.h"
//#include "StdAfx.h"
//#include "Sprite.h"
//#include "resource.h"
//#include "MoteurWindows.h"
//#include "util.h"
//namespace PM3D
//{
//
//	struct ShadersParams
//	{
//		DirectX::XMMATRIX matWVP; // la matrice totale
//	};
//
//#pragma region SommetSprite
//	// Definir l�organisation de notre sommet
//	D3D11_INPUT_ELEMENT_DESC CSommetSprite::layout[] =
//	{
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
//	D3D11_INPUT_PER_VERTEX_DATA, 0},
//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
//	D3D11_INPUT_PER_VERTEX_DATA, 0}
//	};
//	UINT CSommetSprite::numElements = ARRAYSIZE(layout);
//#pragma endregion
//
//
//
//#pragma region CSprite
//	CAfficheurSprite::CAfficheurSprite(CDispositifD3D11* _pDispositif)
//		: pDispositif(_pDispositif)
//		, pVertexBuffer(nullptr)
//		, pConstantBuffer(nullptr)
//		, pEffet(nullptr)
//		, pTechnique(nullptr)
//		, pPasse(nullptr)
//		, pVertexLayout(nullptr)
//		, pSampleState(nullptr)
//	{
//		// Cr�ation du vertex buffer et copie des sommets
//		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();
//		D3D11_BUFFER_DESC bd;
//		ZeroMemory(&bd, sizeof(bd));
//		bd.Usage = D3D11_USAGE_IMMUTABLE;
//		bd.ByteWidth = sizeof(sommets);
//		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		bd.CPUAccessFlags = 0;
//		D3D11_SUBRESOURCE_DATA InitData;
//		ZeroMemory(&InitData, sizeof(InitData));
//		InitData.pSysMem = sommets;
//		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer),
//			DXE_CREATIONVERTEXBUFFER);
//		// Initialisation de l�effet
//		InitEffet();
//	}
//
//	CAfficheurSprite::~CAfficheurSprite()
//	{
//		DXRelacher(pConstantBuffer);
//		DXRelacher(pSampleState);
//		DXRelacher(pEffet);
//		DXRelacher(pVertexLayout);
//		DXRelacher(pVertexBuffer);
//	}
//
//
//	CSommetSprite CAfficheurSprite::sommets[6] = {
//		CSommetSprite(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)),
//		CSommetSprite(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f)),
//		CSommetSprite(DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)),
//		CSommetSprite(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f)),
//		CSommetSprite(DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f)),
//		CSommetSprite(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f))
//	};
//
//	void CAfficheurSprite::InitEffet()
//	{
//		// Compilation et chargement du vertex shader
//		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();
//		// Cr�ation d�un tampon pour les constantes de l�effet
//		D3D11_BUFFER_DESC bd;
//		ZeroMemory(&bd, sizeof(bd));
//		bd.Usage = D3D11_USAGE_DEFAULT;
//		bd.ByteWidth = sizeof(ShadersParams);
//		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//		bd.CPUAccessFlags = 0;
//		pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);
//		// Pour l�effet
//		ID3DBlob* pFXBlob = nullptr;
//		DXEssayer(D3DCompileFromFile(L"Sprite1.fx", 0, 0, 0,
//			"fx_5_0", 0, 0,
//			&pFXBlob, 0),
//			DXE_ERREURCREATION_FX);
//		D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(),
//			pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);
//		pFXBlob->Release();
//		pTechnique = pEffet->GetTechniqueByIndex(0);
//		pPasse = pTechnique->GetPassByIndex(0);
//		// Cr�er l�organisation des sommets pour le VS de notre effet
//		D3DX11_PASS_SHADER_DESC effectVSDesc;
//		pPasse->GetVertexShaderDesc(&effectVSDesc);
//		D3DX11_EFFECT_SHADER_DESC effectVSDesc2; effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex,
//			&effectVSDesc2);
//		const void* vsCodePtr = effectVSDesc2.pBytecode;
//		const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;
//		pVertexLayout = nullptr;
//		DXEssayer(pD3DDevice->CreateInputLayout(
//			CSommetSprite::layout,
//			CSommetSprite::numElements,
//			vsCodePtr,
//			vsCodeLen,
//			&pVertexLayout),
//			DXE_CREATIONLAYOUT);
//		// Initialisation des param�tres de sampling de la texture
//		D3D11_SAMPLER_DESC samplerDesc;
//		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
//		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//		samplerDesc.MipLODBias = 0.0f;
//		samplerDesc.MaxAnisotropy = 4;
//		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//		samplerDesc.BorderColor[0] = 0;
//		samplerDesc.BorderColor[1] = 0;
//		samplerDesc.BorderColor[2] = 0;
//		samplerDesc.BorderColor[3] = 0;
//		samplerDesc.MinLOD = 0;
//		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
//		// Cr�ation de l��tat de sampling
//		pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
//	}
//
//	void CAfficheurSprite::Draw()
//	{
//		// Obtenir le contexte
//		ID3D11DeviceContext* pImmediateContext =
//			pDispositif->GetImmediateContext();
//		// Choisir la topologie des primitives
//		pImmediateContext->IASetPrimitiveTopology(
//			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		// Source des sommets
//		UINT stride = sizeof(CSommetSprite);
//		const UINT offset = 0;
//		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride,
//			&offset);
//		// input layout des sommets
//		pImmediateContext->IASetInputLayout(pVertexLayout);
//		// Le sampler state
//		ID3DX11EffectSamplerVariable* variableSampler;
//		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
//		variableSampler->SetSampler(0, pSampleState);
//		ID3DX11EffectConstantBuffer* pCB =
//			pEffet->GetConstantBufferByName("param");
//		ID3DX11EffectShaderResourceVariable* variableTexture;
//		variableTexture =
//			pEffet->GetVariableByName("textureEntree")->AsShaderResource();
//		pDispositif->ActiverMelangeAlpha();
//		// Faire le rendu de tous nos sprites
//		for (int i = 0; i < tabSprites.size(); ++i)
//		{
//			// Initialiser et s�lectionner les � constantes � de l�effet
//			ShadersParams sp;
//			sp.matWVP = XMMatrixTranspose(tabSprites[i]->matPosDim);
//			pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr,
//				&sp, 0, 0);
//			pCB->SetConstantBuffer(pConstantBuffer);
//			// Activation de la texture
//			variableTexture->SetResource(tabSprites[i]->pTextureD3D);
//			pPasse->Apply(0, pImmediateContext);
//			// **** Rendu de l�objet
//			pImmediateContext->Draw(6, 0);
//		}
//		pDispositif->DesactiverMelangeAlpha();
//	}
//
//	
//
//
//	void CAfficheurSprite::AjouterSprite(const std::string& NomTexture,
//		int _x, int _y,
//		int _dx, int _dy)
//	{
//		float x, y, dx, dy;
//		float posX, posY;
//		float facteurX, facteurY;
//		// Initialisation de la texture
//		CGestionnaireDeTextures& TexturesManager =
//			CMoteurWindows::GetInstance().GetTextureManager();
//		std::wstring ws(NomTexture.begin(), NomTexture.end());
//		std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();;
//		pSprite->pTextureD3D =
//			TexturesManager.GetNewTexture(ws.c_str(), pDispositif) -> GetD3DTexture();
//		// Obtenir les dimensions de la texture si _dx et _dy sont � 0;
//		if (_dx == 0 && _dy == 0)
//		{
//			ID3D11Resource* pResource;
//			ID3D11Texture2D* pTextureInterface = 0;
//			pSprite->pTextureD3D->GetResource(&pResource);
//			pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
//			D3D11_TEXTURE2D_DESC desc;
//			pTextureInterface->GetDesc(&desc);
//			DXRelacher(pResource);
//			DXRelacher(pTextureInterface);
//			dx = float(desc.Width);
//			dy = float(desc.Height);
//		}
//		else
//		{
//			dx = float(_dx);
//			dy = float(_dy);
//		}
//		// Dimension en facteur
//		facteurX = dx * 2.0f / pDispositif->GetLargeur();
//		facteurY = dy * 2.0f / pDispositif->GetHauteur();
//		// Position en coordonn�es logiques
//		// 0,0 pixel = -1,1
//		x = float(_x);
//		y = float(_y);
//		posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
//		posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();
//		pSprite->matPosDim = DirectX::XMMatrixScaling(facteurX, facteurY, 1.0f) *
//			DirectX::XMMatrixTranslation(posX, posY, 0.0f);
//		// On l�ajoute � notre vecteur
//		tabSprites.push_back(std::move(pSprite));
//	}
//
//
//	void CAfficheurSprite::AjouterPanneau(const std::string& NomTexture,
//		const DirectX::XMFLOAT3& _position,
//		float _dx, float _dy)
//	{
//		// Initialisation de la texture
//		CGestionnaireDeTextures& TexturesManager =
//			CMoteurWindows::GetInstance().GetTextureManager();
//		std::wstring ws(NomTexture.begin(), NomTexture.end());
//		std::unique_ptr<CPanneau> pPanneau = std::make_unique<CPanneau>(); 
//		pPanneau->pTextureD3D =	TexturesManager.GetNewTexture(ws.c_str(), pDispositif)-> GetD3DTexture();
//
//		// Obtenir la dimension de la texture si _dx et _dy sont � 0;
//		if (_dx == 0.0f && _dy == 0.0f)
//		{
//			ID3D11Resource* pResource;
//			ID3D11Texture2D* pTextureInterface = 0;
//			pPanneau->pTextureD3D->GetResource(&pResource);
//			pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
//			D3D11_TEXTURE2D_DESC desc;
//			pTextureInterface->GetDesc(&desc);
//			DXRelacher(pResource);
//			DXRelacher(pTextureInterface);
//			pPanneau->dimension.x = float(desc.Width);
//			pPanneau->dimension.y = float(desc.Height);
//			// Dimension en facteur
//			pPanneau->dimension.x = pPanneau->dimension.x * 2.0f / pDispositif-> GetLargeur();
//			pPanneau->dimension.y = pPanneau->dimension.y * 2.0f / pDispositif-> GetHauteur();
//		}
//		else
//		{
//			pPanneau->dimension.x = float(_dx);
//			pPanneau->dimension.y = float(_dy);
//		}
//		// Position en coordonn�es du monde
//		//const DirectX::XMMATRIX& viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
//		//const DirectX::XMMATRIX& viewProj = CMoteurWindows::GetInstance().GetCurrentScene()->GetCamera(); // ?????????????????
//		pPanneau->position = _position;
//		pPanneau->matPosDim = DirectX::XMMatrixScaling(pPanneau->dimension.x,
//			pPanneau->dimension.y, 1.0f) *
//			DirectX::XMMatrixTranslation(pPanneau->position.x,
//				pPanneau->position.y, pPanneau->position.z) *
//			viewProj;
//		// On l�ajoute � notre vecteur
//		tabSprites.push_back(std::move(pPanneau));
//	}
//
//
//#pragma endregion
//}