#include "stdafx.h"
#include "ObjetMesh.h"
#include "moteurWindows.h" 
#include "util.h" 
#include "resource.h"
#include <string>
#include "ChargeurOBJ.h"

using namespace DirectX;
using namespace std;

namespace PM3D
{
	#pragma region CSommetMesh

	// Definir l'organisation de notre sommet
	D3D11_INPUT_ELEMENT_DESC CObjetMesh::CSommetMesh::layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	UINT CObjetMesh::CSommetMesh::numElements;

	//CObjetMesh::CSommetMesh::CSommetMesh(XMFLOAT3 _position, XMFLOAT3 _normal, XMFLOAT2 _coordTex) 
	//	: position{_position}, normal{_normal}, coordTex{_coordTex}
	//{
	//}

	#pragma endregion

	#pragma region ObjectMesh

	CObjetMesh::CObjetMesh(const IChargeur& chargeur) : CObjet3D()
	{
		// Placer l'objet sur la carte graphique
		TransfertObjet(chargeur);

		// Initialisation de l'effet
		InitEffet();
	}

	// Constructeur de conversion
	// Constructeur pour test ou pour création d'un objet de format OMB
	CObjetMesh::CObjetMesh(const IChargeur& chargeur, const std::string& nomfichier) : CObjet3D()
	{
		//// Placer l'objet sur la carte graphique
		// TransfertObjet(chargeur); // On n'utilisera plus cette fonction

		EcrireFichierBinaire(chargeur, nomfichier);

		LireFichierBinaire(nomfichier);

		// Initialisation de l'effet
		InitEffet();
	}

	// Constructeur pour lecture d'un objet de format OMB
	CObjetMesh::CObjetMesh(const std::string& nomfichier) : CObjet3D()
	{
		// Placer l'objet sur la carte graphique
		LireFichierBinaire(nomfichier);

		// Initialisation de l'effet
		InitEffet();
	}

	CObjetMesh::~CObjetMesh()
	{
		//DXRelacher(pConstantBuffer); 
		//DXRelacher(pSampleState);
		//DXRelacher(pEffet);
		//DXRelacher(pVertexLayout); 
		//DXRelacher(pIndexBuffer); 
		//DXRelacher(pVertexBuffer);

		//// Ombres
		//DXRelacher(pDepthStencilView);
		//DXRelacher(pDepthShaderResourceView);
		//DXRelacher(pDepthTexture);
		//DXRelacher(pVertexLayoutShadow);

		SubmeshMaterialIndex.clear();
		SubmeshIndex.clear();
		Material.clear();

		DXRelacher(pConstantBuffer);
		DXRelacher(pSampleState);

		DXRelacher(pEffet);
		DXRelacher(pVertexLayout);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);

		DXRelacher(pShadowMapView);
		DXRelacher(pRenderTargetView);
		DXRelacher(pTextureShadowMap);
		DXRelacher(pDepthStencilView);
		DXRelacher(pDepthTexture);
		DXRelacher(pVertexLayoutShadow);
	}

	void CObjetMesh::InitEffet()
	{
		// Compilation et chargement du vertex shader 
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		// Cr�ation d�un tampon pour les constantes du VS 
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		HRESULT hr = pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

		//DXEssayer(pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer));

		// Pour l�effet 
		ID3DBlob* pFXBlob = nullptr;
		//DXEssayer(D3DCompileFromFile(L"resources/MiniPhongMesh.fx", 0, 0, 0, "fx_5_0", 0, 0, &pFXBlob, 0),
		//	DXE_ERREURCREATION_FX);

		DXEssayer(D3DCompileFromFile(L"resources/MiniPhongMesh.fx", 0, 0, 0,
			"fx_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pFXBlob, 0),
			DXE_ERREURCREATION_FX);

		D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);
		pFXBlob->Release();

		pTechnique = pEffet->GetTechniqueByIndex(0);
		pPasse = pTechnique->GetPassByIndex(0);

		// Cr�er l�organisation des sommets pour les VS de notre effet
		D3DX11_PASS_SHADER_DESC effectVSDesc;
		D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
		const void* vsCodePtr;
		unsigned vsCodeLen;
		CSommetMesh::numElements = ARRAYSIZE(CSommetMesh::layout);
		// 1 pour le shadowmap
		pTechnique = pEffet->GetTechniqueByName("ShadowMap");
		pPasse = pTechnique->GetPassByIndex(0);
		pPasse->GetVertexShaderDesc(&effectVSDesc);
		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex,
			&effectVSDesc2);
		vsCodePtr = effectVSDesc2.pBytecode;
		vsCodeLen = effectVSDesc2.BytecodeLength;
		pVertexLayout = nullptr;

		DXEssayer(pD3DDevice->CreateInputLayout(CSommetMesh::layout,
			CSommetMesh::numElements,
			vsCodePtr,
			vsCodeLen,
			&pVertexLayoutShadow),
			DXE_CREATIONLAYOUT);
		// 2 pour miniphong
		pTechnique = pEffet->GetTechniqueByName("MiniPhong");
		pPasse = pTechnique->GetPassByIndex(0);
		pPasse->GetVertexShaderDesc(&effectVSDesc);
		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex,
			&effectVSDesc2);
		vsCodePtr = effectVSDesc2.pBytecode;
		vsCodeLen = effectVSDesc2.BytecodeLength;
		pVertexLayout = nullptr;
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetMesh::layout,
			CSommetMesh::numElements,
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

		// Cr�ation de l��tat de sampling 
		pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);

		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		// Description de la texture
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		// Cette texture sera utilisée comme cible de rendu et 
		// comme ressource de shader
		textureDesc.Width = SHADOWMAP_DIM;
		textureDesc.Height = SHADOWMAP_DIM;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// Création de la texture
		pD3DDevice->CreateTexture2D(&textureDesc, nullptr, &pTextureShadowMap);

		// VUE - Cible de rendu
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// Création de la vue.
		pD3DDevice->CreateRenderTargetView(pTextureShadowMap,
			&renderTargetViewDesc,
			&pRenderTargetView);

		// VUE – Ressource de shader
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		// Création de la vue.
		pD3DDevice->CreateShaderResourceView(pTextureShadowMap,
			&shaderResourceViewDesc,
			&pShadowMapView);

		InitDepthBuffer();
		//InitMatricesShadowMap();


		//// Cr�ation des objets
		//InitDepthBuffer();

		//InitMatricesShadowMap();
	}

	void CObjetMesh::Draw(const CCamera& camera, const CLight& light, const CTransform& transform)
	{
		#pragma region comments
			/*
			// Obtenir le contexte 
			ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

			// Choisir la topologie des primitives 
			pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		
			// input layout des sommets 
			pImmediateContext->IASetInputLayout( pVertexLayout );

			// Index buffer 
			pImmediateContext->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			// Vertex buffer 
			UINT stride = sizeof(CSommetMesh); 
			UINT offset = 0; 
			pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

			// Initialiser et s�lectionner les � constantes � de l�effet 
			ShadersParams sp{};
			XMMATRIX viewProj = camera.GetMatViewProj();
			XMMATRIX matWorld = transform.GetWorldMatrix();

			sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
			sp.matWorld = XMMatrixTranspose(matWorld);
			sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -15.0f, 1.0f); 
			sp.vCamera = XMVectorSet(0.0f, 3.0f, -5.0f, 1.0f); 
			sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f); 
			sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
			sp.vSEcl = XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f);

			// Le sampler state 
			ID3DX11EffectSamplerVariable* variableSampler; 
			variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler(); 
			variableSampler->SetSampler(0, pSampleState);

			// Dessiner les sous-objets non-transparents
			for (int i = 0; i < NombreSubmesh; ++i)
			{
				int indexStart = SubmeshIndex[i]; 
				int indexDrawAmount = SubmeshIndex[i + 1] - SubmeshIndex[i];

				if (indexDrawAmount) 
				{
					sp.vAMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Ambient);
					sp.vDMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Diffuse);
					sp.vSMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Specular); 
					sp.puissance = Material[SubmeshMaterialIndex[i]].Puissance; 
				
					// Activation de la texture ou non 
					if (Material[SubmeshMaterialIndex[i]].pTextureD3D != nullptr) 
					{ 
						ID3DX11EffectShaderResourceVariable* variableTexture; 
						variableTexture = pEffet->GetVariableByName("textureEntree")->AsShaderResource(); 
						variableTexture->SetResource(Material[SubmeshMaterialIndex[i]].pTextureD3D); 
						sp.bTex = 1; 
					} 
					else 
					{ 
						sp.bTex = 0; 
					} 
				
					// IMPORTANT pour ajuster les param. 
					pPasse->Apply(0, pImmediateContext); 
				
					// Nous n�avons qu�un seul CBuffer 
					ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");
					pCB->SetConstantBuffer(pConstantBuffer); 
					pImmediateContext->UpdateSubresource( pConstantBuffer, 0, nullptr, &sp, 0, 0 );
				
					pImmediateContext->DrawIndexed( indexDrawAmount, indexStart, 0 ); }
			}
			*/

			//// ***** OMBRES ---- Valide pour les deux rendus

			//// Obtenir le contexte
			//ID3D11DeviceContext* pImmediateContext = pDispositif -> GetImmediateContext();
			//// Choisir la topologie des primitives
			//pImmediateContext->IASetPrimitiveTopology(
			//	D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//// Index buffer
			//pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT,
			//	0);
			//// Vertex buffer
			//UINT stride = sizeof(CSommetMesh);
			//UINT offset = 0;
			//pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride,
			//	&offset);

			//XMMATRIX matWorld = transform.GetWorldMatrix();

			//// ***** OMBRES ---- Premier Rendu - Cr�ation du Shadow Map
			//
			//// Utiliser la surface de la texture comme surface de rendu
			//pImmediateContext->OMSetRenderTargets(0, nullptr,
			//	pDepthStencilView);
			//// Effacer le shadow map
			//pImmediateContext -> ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
			//// Modifier les dimension du viewport
			//pDispositif->SetViewPortDimension(512, 512);
			//// Choix de la technique
			//pTechnique = pEffet->GetTechniqueByName("ShadowMap");
			//pPasse = pTechnique->GetPassByIndex(0);
			//// input layout des sommets
			//pImmediateContext->IASetInputLayout(pVertexLayoutShadow);
			//// Initialiser et s�lectionner les � constantes � de l�effet
			//ShadersParams sp;
			//sp.matWorldViewProjLight = XMMatrixTranspose(matWorld * mVPLight);
			//// Nous n�avons qu�un seul CBuffer
			//ID3DX11EffectConstantBuffer* pCB =
			//	pEffet->GetConstantBufferByName("param");
			//pCB->SetConstantBuffer(pConstantBuffer);
			//pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp,
			//	0, 0);
			//// Dessiner les subsets non-transparents
			//for (int i = 0; i < NombreSubmesh; ++i) // p314 ?
			//{
			//	const int indexStart = SubmeshIndex[i];
			//	const int indexDrawAmount = SubmeshIndex[i + 1] - SubmeshIndex[i];
			//	if (indexDrawAmount)
			//	{
			//		// IMPORTANT pour ajuster les param.
			//		pPasse->Apply(0, pImmediateContext);
			//		pImmediateContext->DrawIndexed(indexDrawAmount, indexStart, 0);
			//	}
			//}

			//// ***** OMBRES ---- Deuxi�me Rendu - Affichage de l�objet avec ombres

			//// Ramener la surface de rendu
			//ID3D11RenderTargetView* tabRTV[1];
			//tabRTV[0] = pDispositif->GetRenderTargetView();
			//pImmediateContext->OMSetRenderTargets(1,
			//	tabRTV,
			//	pDispositif->GetDepthStencilView());
			//// Dimension du viewport - d�faut
			//pDispositif->ResetViewPortDimension();
			//// Choix de la technique
			//pTechnique = pEffet->GetTechniqueByName("MiniPhongSM");
			//pPasse = pTechnique->GetPassByIndex(0);
			//// Initialiser et s�lectionner les � constantes � de l�effet
			//XMMATRIX viewProj = camera.GetMatViewProj();
			//sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
			//sp.matWorld = XMMatrixTranspose(matWorld);
			////sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -15.0f, 1.0f);
			//sp.vLumiere = light.GetTransform().GetPositionVector();
			//sp.vCamera = XMVectorSet(0.0f, 3.0f, -5.0f, 1.0f);
			////sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
			////sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
			////sp.vSEcl = XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f);
			//sp.vAEcl = light.GetAmbient();
			//sp.vDEcl = light.GetDiffuse();
			//sp.vSEcl = light.GetSpecular();
			//// Le sampler state
			//ID3DX11EffectSamplerVariable* variableSampler;
			//variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
			//variableSampler->SetSampler(0, pSampleState);
			//// input layout des sommets
			//pImmediateContext->IASetInputLayout(pVertexLayout);
			//ID3DX11EffectShaderResourceVariable* pShadowMap;
			//pShadowMap = pEffet->GetVariableByName("ShadowTexture") -> AsShaderResource();
			//pShadowMap->SetResource(pDepthShaderResourceView);
			//pDispositif->SetNormalRSState();
			//// Dessiner les subsets non-transparents
			//for (int i = 0; i < NombreSubmesh; ++i)
			//{
			//	int indexStart = SubmeshIndex[i];
			//	int indexDrawAmount = SubmeshIndex[i + 1] - SubmeshIndex[i];
			//	if (indexDrawAmount)
			//	{
			//		sp.vAMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Ambient);
			//		sp.vDMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Diffuse);
			//		sp.vSMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Specular);
			//		sp.puissance = Material[SubmeshMaterialIndex[i]].Puissance;
			//		// Activation de la texture ou non
			//		if (Material[SubmeshMaterialIndex[i]].pTextureD3D != nullptr)
			//		{
			//			ID3DX11EffectShaderResourceVariable* variableTexture;
			//			variableTexture =
			//				pEffet->GetVariableByName("textureEntree")->AsShaderResource();
			//			variableTexture -> SetResource(Material[SubmeshMaterialIndex[i]].pTextureD3D);
			//			sp.bTex = 1;
			//		}
			//		else
			//		{
			//			sp.bTex = 1;
			//		}
			//		// IMPORTANT pour ajuster les param.
			//		pPasse->Apply(0, pImmediateContext);
			//		pCB->SetConstantBuffer(pConstantBuffer);
			//		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr,
			//			&sp, 0, 0);
			//		pImmediateContext->DrawIndexed(indexDrawAmount, indexStart, 0);
			//	}
			//}
	// ***** OMBRES ---- Valide pour les deux rendus
		// Obtenir le contexte
		#pragma endregion

		// ***** OMBRES ---- Valide pour les deux rendus
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

		// Choisir la topologie des primitives
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Index buffer
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Vertex buffer
		UINT stride = sizeof(CSommetMesh);
		const UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		// ***** OMBRES ---- Premier Rendu - Création du Shadow Map
		// Utiliser la surface de la texture comme surface de rendu
		//ID3D11RenderTargetView* pRenderTargetView = pDispositif->GetRenderTargetView();
		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView,
			pDepthStencilView);

		// Effacer le shadow map 
		float Couleur[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, Couleur);
		pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		// Modifier les dimension du viewport
		pDispositif->SetViewPortDimension(512, 512);

		// Choix de la technique
		pTechnique = pEffet->GetTechniqueByName("ShadowMap");
		pPasse = pTechnique->GetPassByIndex(0);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayoutShadow);

		// Initialiser et sélectionner les «constantes» de l'effet
		ShadersParams sp;
		XMMATRIX matWorld = transform.GetWorldMatrix();
		XMMATRIX viewProj = camera.GetMatViewProj();

		//sp.matWorldViewProjLight = XMMatrixTranspose(matWorld * mVPLight);
		sp.matWorldViewProjLight = XMMatrixTranspose(matWorld * light.GetMatViewProj());


		//XMMATRIX viewProj = camera.GetMatViewProj();
		//sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		//sp.matWorld = XMMatrixTranspose(matWorld);

		// Initialiser et s�lectionner les � constantes � de l�effet 

		//
		//
		//sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		//sp.matWorld = XMMatrixTranspose(matWorld);
		//sp.matWorldViewProjLight = XMMatrixTranspose(matWorld * mVPLight);

		// Nous n'avons qu'un seul CBuffer
		ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");
		pCB->SetConstantBuffer(pConstantBuffer);
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

		// Dessiner les submeshs non-transparents
		for (int i = 0; i < NombreSubmesh; ++i)
		{
			const int indexStart = SubmeshIndex[i];
			const int indexDrawAmount = SubmeshIndex[i + 1] - SubmeshIndex[i];
			if (indexDrawAmount)
			{
				// IMPORTANT pour ajuster les param.
				pPasse->Apply(0, pImmediateContext);

				pImmediateContext->DrawIndexed(indexDrawAmount, indexStart, 0);
			}
		}

		// ***** OMBRES ---- Deuxième Rendu - Affichage de l'objet avec ombres
			// Ramener la surface de rendu
		ID3D11RenderTargetView* tabRTV[1];
		tabRTV[0] = pDispositif->GetRenderTargetView();
		pImmediateContext->OMSetRenderTargets(1,
			tabRTV,
			pDispositif->GetDepthStencilView());

		// Dimension du viewport - défaut
		pDispositif->ResetViewPortDimension();

		// Choix de la technique
		pTechnique = pEffet->GetTechniqueByName("MiniPhong");
		pPasse = pTechnique->GetPassByIndex(0);
		

		// Update les paramètres du buffer
		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		sp.matWorld = XMMatrixTranspose(matWorld);

		sp.vDirLum = light.GetLightDirection(transform);
		sp.vCamera = camera.GetTransform().GetPositionVector();
		sp.vAEcl = light.GetAmbient();
		sp.vDEcl = light.GetDiffuse();
		sp.vSEcl = light.GetSpecular();

		// Le sampler state
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);


		ID3DX11EffectShaderResourceVariable* pShadowMap;
		pShadowMap = pEffet->GetVariableByName("ShadowTexture")->AsShaderResource();
		//pShadowMap->SetResource(pDepthShaderResourceView);
		pShadowMap->SetResource(pShadowMapView);

		pDispositif->SetNormalRSState();

		// Dessiner les submeshs non-transparents
		for (int i = 0; i < NombreSubmesh; ++i)
		{
			const int indexStart = SubmeshIndex[i];
			const int indexDrawAmount = SubmeshIndex[i + 1] - SubmeshIndex[i];
			if (indexDrawAmount)
			{
				sp.vAMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Ambient);
				sp.vDMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Diffuse);
				sp.vSMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Specular);
				sp.puissance = Material[SubmeshMaterialIndex[i]].Puissance;

				// Activation de la texture ou non
				if (Material[SubmeshMaterialIndex[i]].pTextureD3D != nullptr)
				{
					ID3DX11EffectShaderResourceVariable* variableTexture;
					variableTexture =
						pEffet->GetVariableByName("textureEntree")->AsShaderResource();
					variableTexture->SetResource(Material[SubmeshMaterialIndex[i]].pTextureD3D);
					sp.bTex = 1;
				}
				else
				{
					sp.bTex = 0;
				}

				// IMPORTANT pour ajuster les param.
				pPasse->Apply(0, pImmediateContext);

				pCB->SetConstantBuffer(pConstantBuffer);
				pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

				pImmediateContext->DrawIndexed(indexDrawAmount, indexStart, 0);
			}
		}
	}

	void CObjetMesh::TransfertObjet(const IChargeur& chargeur)
	{
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		// 1. SOMMETS a) Cr�ations des sommets dans un tableau temporaire
		{
			const size_t nombreSommets = chargeur.GetNombreSommets();
			/*unique_ptr<CSommetMesh[]> */ts = std::unique_ptr<CSommetMesh[]>(new CSommetMesh[nombreSommets]);

			for (uint32_t i = 0; i < nombreSommets; ++i)
			{
				ts[i].position = chargeur.GetPosition(i);
				ts[i].normal = chargeur.GetNormale(i);
				ts[i].coordTex = chargeur.GetCoordTex(i);
			}

			// 1. SOMMETS b) Cr�ation du vertex buffer et copie des sommets
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = static_cast<uint32_t>(sizeof(CSommetMesh) * nombreSommets);
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = ts.get();
			pVertexBuffer = nullptr;
			DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);
		}

		// 2. INDEX -	Cr�ation de l�index buffer et copie des indices 
		//				Les indices �tant habituellement des entiers, j�ai 
		//				pris directement ceux du chargeur, mais attention au 
		//				format si vous avez autre chose que DXGI_FORMAT_R32_UINT
		{
			D3D11_BUFFER_DESC bd; 
			ZeroMemory(&bd, sizeof(bd)); 
			
			bd.Usage = D3D11_USAGE_IMMUTABLE; 
			bd.ByteWidth = static_cast<uint32_t>(sizeof(uint32_t) * chargeur.GetNombreIndex()); 
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER; 
			bd.CPUAccessFlags = 0; 
			
			D3D11_SUBRESOURCE_DATA InitData; 
			ZeroMemory(&InitData, sizeof(InitData)); 
			InitData.pSysMem = chargeur.GetIndexData(); 

			index = chargeur.GetIndex();

			pIndexBuffer = nullptr; 
			
			DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer), DXE_CREATIONINDEXBUFFER);
		}

		// 3. Les sous-objets
		NombreSubmesh = chargeur.GetNombreSubmesh();

		// D�but de chaque sous-objet et un pour la fin
		SubmeshIndex.reserve(NombreSubmesh);
		chargeur.CopieSubmeshIndex(SubmeshIndex);

		// 4. MATERIAUX
		{
			//	4a) Cr�er un mat�riau de d�faut en index 0 
			//		Vous pourriez changer les valeurs, j�ai conserv� 
			//		celles du constructeur
			Material.reserve(chargeur.GetNombreMaterial() + 1);
			Material.emplace_back(CMaterial());

			// 4b) Copie des mat�riaux dans la version locale 
			for (int32_t i = 0; i < chargeur.GetNombreMaterial(); ++i)
			{
				CMaterial mat;
				chargeur.GetMaterial(i, mat.NomFichierTexture,
					mat.NomMateriau,
					mat.Ambient,
					mat.Diffuse,
					mat.Specular,
					mat.Puissance);
				Material.push_back(mat);
			}

			// 4c) Trouver l�index du materiau pour chaque sous-ensemble 
			SubmeshMaterialIndex.reserve(chargeur.GetNombreSubmesh());
			for (int32_t i = 0; i < chargeur.GetNombreSubmesh(); ++i) 
			{ 
				int32_t index; 
				for (index = 0; index < Material.size(); ++index) 
				{ 
					if (Material[index].NomMateriau == chargeur.GetMaterialName(i)) 
						break; 
				} 
				if (index >= Material.size()) index = 0;		// valeur de d�faut 
				SubmeshMaterialIndex.push_back(index);
			}

			// 4d) Chargement des textures 
			CGestionnaireDeTextures& TexturesManager = CMoteurWindows::GetInstance().GetTextureManager(); 
			for (uint32_t i = 0; i < Material.size(); ++i) 
			{ 
				if (Material[i].NomFichierTexture.length() > 0) 
				{ 
					const std::wstring ws(Material[i].NomFichierTexture.begin(), Material[i].NomFichierTexture.end()); 
					Material[i].pTextureD3D = TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture(); 
				} 
			}
		}
	}

	void CObjetMesh::EcrireFichierBinaire(const IChargeur& chargeur, const std::string& nomFichier)
	{
		std::ofstream fichier;
		fichier.open(nomFichier, std::ios::out | std::ios_base::binary);
		// 1. SOMMETS a) Créations des sommets dans un tableau temporaire
		{
			int32_t nombreSommets = static_cast<int32_t>(chargeur.GetNombreSommets());
			std::unique_ptr<CSommetMesh[]> ts(new CSommetMesh[nombreSommets]);

			for (int32_t i = 0; i < nombreSommets; ++i)
			{
				ts[i].position = chargeur.GetPosition(i);
				ts[i].normal = chargeur.GetNormale(i);
				ts[i].coordTex = chargeur.GetCoordTex(i);
			}

			// 1. SOMMETS b) Écriture des sommets dans un fichier binaire
			fichier.write((char*)&nombreSommets, sizeof(nombreSommets));
			fichier.write((char*)ts.get(), nombreSommets * sizeof(CSommetMesh));
		}

		// 2. INDEX 
		const int32_t nombreIndex = static_cast<int32_t>(chargeur.GetNombreIndex());

		fichier.write((char*)&nombreIndex, sizeof(nombreIndex));
		fichier.write((char*)chargeur.GetIndexData(), nombreIndex * sizeof(uint32_t));

		// 3. Les sous-objets
		const int32_t NombreSubmesh = chargeur.GetNombreSubmesh();

		//    Début de chaque sous-objet et un pour la fin
		std::vector<int32_t> SI;
		SI.reserve(NombreSubmesh);
		chargeur.CopieSubmeshIndex(SI);

		fichier.write((char*)&NombreSubmesh, sizeof(NombreSubmesh));
		fichier.write((char*)SI.data(), (NombreSubmesh + 1) * sizeof(int32_t));

		// 4. MATERIAUX
		// 4a) Créer un matériau de défaut en index 0
		//     Vous pourriez changer les valeurs, j'ai conservé 
		//     celles du constructeur
		int32_t NbMaterial = static_cast<int32_t>(chargeur.GetNombreMaterial());
		std::vector<CMaterial> MatLoad;
		MatLoad.reserve(NbMaterial + 1);
		MatLoad.emplace_back(CMaterial());

		// 4b) Copie des matériaux dans la version locale
		CMaterial mat;
		for (int32_t i = 0; i < NbMaterial; ++i)
		{
			chargeur.GetMaterial(i, mat.NomFichierTexture,
				mat.NomMateriau,
				mat.Ambient,
				mat.Diffuse,
				mat.Specular,
				mat.Puissance);

			MatLoad.push_back(mat);
		}

		NbMaterial++;
		fichier.write((char*)&NbMaterial, sizeof(int32_t));

		MaterialBlock mb;
		for (int32_t i = 0; i < NbMaterial; ++i)
		{
			MatLoad[i].MatToBlock(mb);
			fichier.write((char*)&mb, sizeof(MaterialBlock));
		}

		// 4c) Trouver l'index du materiau pour chaque sous-ensemble
		std::vector<int32_t> SubsetMI;
		SubsetMI.reserve(NombreSubmesh);
		for (int32_t i = 0; i < NombreSubmesh; ++i)
		{
			int32_t index;
			for (index = 0; index < MatLoad.size(); ++index)
			{
				if (MatLoad[index].NomMateriau == chargeur.GetMaterialName(i)) break;
			}

			if (index >= MatLoad.size()) index = 0;  // valeur de défaut

			SubsetMI.push_back(index);
		}

		fichier.write((char*)SubsetMI.data(), (NombreSubmesh) * sizeof(int32_t));
	}

	void CObjetMesh::LireFichierBinaire(const std::string& nomFichier)
	{
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		std::ifstream fichier;
		fichier.open(nomFichier, std::ios::in | std::ios_base::binary);
		assert(fichier.is_open());

		// 1. SOMMETS a) Créations des sommets dans un tableau temporaire
		{
			int32_t nombreSommets;
			fichier.read((char*)&nombreSommets, sizeof(nombreSommets));

			std::unique_ptr<CSommetMesh[]> ts(new CSommetMesh[nombreSommets]);

			// 1. SOMMETS b) Lecture des sommets à partir d'un fichier binaire
			fichier.read((char*)ts.get(), nombreSommets * sizeof(CSommetMesh));

			// 1. SOMMETS b) Création du vertex buffer et copie des sommets
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = sizeof(CSommetMesh) * nombreSommets;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = ts.get();
			pVertexBuffer = nullptr;

			DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);
		}

		// 2. INDEX 
		{
			int32_t nombreIndex;
			fichier.read((char*)&nombreIndex, sizeof(nombreIndex));

			std::unique_ptr<uint32_t[]> index(new uint32_t[nombreIndex]);
			fichier.read((char*)index.get(), nombreIndex * sizeof(uint32_t));

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));

			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = sizeof(uint32_t) * nombreIndex;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = index.get();
			pIndexBuffer = nullptr;

			DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
				DXE_CREATIONINDEXBUFFER);
		}

		// 3. Les sous-objets
		fichier.read((char*)&NombreSubmesh, sizeof(NombreSubmesh));
		//    Début de chaque sous-objet et un pour la fin
		{
			std::unique_ptr<int32_t[]> si(new int32_t[NombreSubmesh + 1]);

			fichier.read((char*)si.get(), (NombreSubmesh + 1) * sizeof(int32_t));
			SubmeshIndex.assign(si.get(), si.get() + (NombreSubmesh + 1));
		}

		// 4. MATERIAUX
		// 4a) Créer un matériau de défaut en index 0
		//     Vous pourriez changer les valeurs, j'ai conservé 
		//     celles du constructeur
		CMaterial mat;

		int32_t NbMaterial;
		fichier.read((char*)&NbMaterial, sizeof(int32_t));

		Material.resize(NbMaterial);

		MaterialBlock mb;
		for (int32_t i = 0; i < NbMaterial; ++i)
		{
			fichier.read((char*)&mb, sizeof(MaterialBlock));
			Material[i].BlockToMat(mb);
		}

		// 4c) Trouver l'index du materiau pour chaque sous-ensemble
		{
			std::unique_ptr<int32_t[]> smi(new int32_t[NombreSubmesh]);

			fichier.read((char*)smi.get(), (NombreSubmesh) * sizeof(int32_t));
			SubmeshMaterialIndex.assign(smi.get(), smi.get() + NombreSubmesh);
		}

		// 4d) Chargement des textures
		CGestionnaireDeTextures& TexturesManager = CMoteurWindows::GetInstance().GetTextureManager();

		for (uint32_t i = 0; i < Material.size(); ++i)
		{
			if (Material[i].NomFichierTexture.length() > 0)
			{
				std::wstring ws(Material[i].NomFichierTexture.begin(), Material[i].NomFichierTexture.end());
				Material[i].pTextureD3D = TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();
			}
		}
	}

	void CObjetMesh::InitDepthBuffer()
	{
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();
		D3D11_TEXTURE2D_DESC depthTextureDesc;
		ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
		depthTextureDesc.Width = 512;
		depthTextureDesc.Height = 512;
		depthTextureDesc.MipLevels = 1;
		depthTextureDesc.ArraySize = 1;
		depthTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthTextureDesc.SampleDesc.Count = 1;
		depthTextureDesc.SampleDesc.Quality = 0;
		depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE |
			D3D11_BIND_DEPTH_STENCIL;
		depthTextureDesc.CPUAccessFlags = 0;
		depthTextureDesc.MiscFlags = 0;

		DXEssayer(
			pD3DDevice->CreateTexture2D(&depthTextureDesc, nullptr,
				&pDepthTexture),
			DXE_ERREURCREATIONTEXTURE);

		// Cr�ation de la vue du tampon de profondeur (ou de stencil)
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSView;
		ZeroMemory(&descDSView, sizeof(descDSView));
		descDSView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSView.Texture2D.MipSlice = 0;
		DXEssayer(
			pD3DDevice->CreateDepthStencilView(pDepthTexture, &descDSView,
				&pDepthStencilView),
			DXE_ERREURCREATIONDEPTHSTENCILTARGET);

		//// Cr�ation d�une shader resource view pour lire le tampon de profondeur dans le shader.
		//	D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
		//sr_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		//sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//sr_desc.Texture2D.MostDetailedMip = 0;
		//sr_desc.Texture2D.MipLevels = 1;
		//DXEssayer(
		//	pD3DDevice->CreateShaderResourceView(pDepthTexture, &sr_desc,
		//		&pDepthShaderResourceView),
		//	DXE_ERREURCREATIONSRV);
	}
	/*
	void CObjetMesh::InitMatricesShadowMap()
	{
		
		// Matrice de la vision vu par la lumière - Le point TO est encore 0,0,0
		mVLight = XMMatrixLookAtLH(
			XMVectorSet(-5.0f, 5.0f, -5.0f, 1.0f),
			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
			XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));

		const float champDeVision = XM_PI / 4;  // 45 degrés
		const float ratioDAspect = 1.0f; 	// 512/512
		const float planRapproche = 2.0f; 	// Pas besoin d'être trop près
		const float planEloigne = 100.0f;	// Suffisemment pour avoir tous les objets
		mPLight = XMMatrixPerspectiveFovLH(champDeVision,
			ratioDAspect,
			planRapproche,
			planEloigne);

		mVPLight = mVLight * mPLight;
	}*/
#pragma endregion

	#pragma region CGroupMeshes
	PM3D::CGroupMeshes::CGroupMeshes(CObjetMesh* obj1)
	{
		//addMesh(1, obj1);
		meshes[1] = obj1;
		nbMeshes = 1;
	}

	PM3D::CGroupMeshes::CGroupMeshes(CObjetMesh* obj1, CObjetMesh* obj2)
	{
		//addMesh(1, obj1);
		meshes[1] = obj1;
		//addMesh(2, obj2);
		meshes[2] = obj2;
		nbMeshes = 2;
	}

	PM3D::CGroupMeshes::CGroupMeshes(CObjetMesh* obj1, CObjetMesh* obj2, CObjetMesh* obj3)
	{
		//addMesh(1, obj1);
		meshes[1] = obj1;
		//addMesh(2, obj2);
		meshes[2] = obj2;
		//addMesh(3, obj3);
		meshes[3] = obj3;
		nbMeshes = 3;
	}

	PM3D::CGroupMeshes::CGroupMeshes(CObjetMesh* obj1, CObjetMesh* obj2, CObjetMesh* obj3, CObjetMesh* obj4)
	{
		//addMesh(1, obj1);
		//addMesh(3, obj3);
		//addMesh(2, obj2);
		//addMesh(4, obj4);
		meshes[1] = obj1;
		meshes[2] = obj2;
		meshes[3] = obj3;
		meshes[4] = obj4;
		nbMeshes = 4;
	}
	CGroupMeshes::~CGroupMeshes()
	{
		for (pair<int, CObjetMesh*> entry : meshes)
		{
			if (entry.second) delete entry.second;
		}
		meshes.clear();
	}

	/*
	void CGroupMeshes::addMesh(int level, CObjetMesh mesh) {
		if (meshes.contains(level)) {
			return;
		}
		meshes.insert(std::pair<int, CObjetMesh>(level, mesh));
		nbMeshes++;
	}*/

	std::map<int, CObjetMesh*> CGroupMeshes::getMeshes() {
		return meshes;
	}

	CObjetMesh* CGroupMeshes::getMesh(int level) {
		return meshes[level];
	}

	int PM3D::CGroupMeshes::getNbOfMeshes()
	{
		return nbMeshes;
	}

	void PM3D::CGroupMeshes::Draw(const CCamera& camera, const CLight& light, const CTransform& transform)
	{
		auto posCam = camera.GetTransform().GetPositionFloat3();
		auto posObj = transform.GetPositionFloat3();

		auto dist = sqrtf(powf(posCam.x - posObj.x, 2) + pow(posCam.y - posObj.y, 2) + pow(posCam.z - posObj.z, 2));
		
		if (dist < LEVEL_1_MAX) {
			if (getMeshes().contains(1)) {
				// afficher le mesh de niveau 1
				getMesh(1)->Draw(camera, light, transform);
			}
		}
		else if (dist > LEVEL_1_MAX) {
			if (getMeshes().contains(2)) {
				// afficher le mesh de niveau 2
				getMesh(2)->Draw(camera, light, transform);
			}
			else if (getMeshes().contains(1)) {
				// afficher le mesh de niveau 1
				getMesh(1)->Draw(camera, light, transform);
			}
		}
		else if (dist > LEVEL_2_MAX) {
			if (getMeshes().contains(3)) {
				// afficher le mesh de niveau 3
				getMesh(3)->Draw(camera, light, transform);
			}
			else if (getMeshes().contains(2)) {
				// afficher le mesh de niveau 2
				getMesh(2)->Draw(camera, light, transform);
			}
			else if (getMeshes().contains(1)) {
				// afficher le mesh de niveau 1
				getMesh(1)->Draw(camera, light, transform);
			}
		}
		else if (dist > LEVEL_3_MAX) {
			if (getMeshes().contains(4)) {
				// afficher le mesh de niveau 4
				getMesh(4)->Draw(camera, light, transform);
			}
			else if (getMeshes().contains(3)) {
				// afficher le mesh de niveau 3
				getMesh(3)->Draw(camera, light, transform);
			}
			else if (getMeshes().contains(2)) {
				// afficher le mesh de niveau 2
				getMesh(2)->Draw(camera, light, transform);
			}
			else if (getMeshes().contains(1)) {
				// afficher le mesh de niveau 1
				getMesh(1)->Draw(camera, light, transform);
			}
		}
		else {
			// Erreur de distance ?
		}
		
	}
#pragma endregion
}