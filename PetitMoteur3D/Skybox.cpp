#include "stdafx.h"
#include "Skybox.h"
#include "util.h"
#include "Resource.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>

using namespace DirectX;
using namespace std;

namespace PM3D
{
	struct ShadersParams
	{
		XMMATRIX matWorldViewProj;	// la matrice totale 
		XMMATRIX matWorld;			// matrice de transformation dans le monde 
		XMVECTOR vDirLum; 			// la direction de la lumière
		XMVECTOR vCamera; 			// la position de la caméra
		XMVECTOR vAEcl; 			// la valeur ambiante de l'éclairage
		XMVECTOR vAMat; 			// la valeur ambiante du matériau
		XMVECTOR vDEcl; 			// la valeur diffuse de l'éclairage 
		XMVECTOR vDMat; 			// la valeur diffuse du matériau 
	};

	Skybox::Skybox(const std::wstring& skybox) : CObjet3D()
		, pVertexBuffer{ nullptr }
		, pIndexBuffer{ nullptr }
		, pConstantBuffer{ nullptr }
		, pEffet{ nullptr }
		, pTechnique{ nullptr }
		, pVertexLayout{ nullptr }
		, vectSommets{}
		, pTexture{ nullptr }
		, pSampleState{ nullptr }
	{
		// Création de l'object 3D
		MakeSkybox(skybox);

		// Initialisation de l'effet
		InitEffet();
	}

	Skybox::~Skybox()
	{
		DXRelacher(pVertexBuffer);
		DXRelacher(pIndexBuffer);
		DXRelacher(pConstantBuffer);
		DXRelacher(pEffet);
		DXRelacher(pVertexLayout);
	}

	void Skybox::Draw(const CCamera& camera, const CLight& light, const CTransform& transform)
	{
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

		// Choisir la topologie des primitives
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Source des sommets
		const UINT stride = sizeof(CSommetBloc);
		const UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		// Source des index
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);

		// Initialiser et sélectionner les «constantes» de l'effet
		ShadersParams sp{};
		XMMATRIX viewProj = camera.GetMatViewProj();
		XMMATRIX matWorld = transform.GetWorldMatrix();

		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		sp.matWorld = XMMatrixTranspose(matWorld);

		sp.vCamera = camera.GetTransform().GetPositionVector();
		sp.vDirLum = light.GetLightDirection(transform);
		sp.vAEcl = light.GetAmbient();
		sp.vDEcl = light.GetDiffuse();
		sp.vAMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

		ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");  // Nous n'avons qu'un seul CBuffer
		pCB->SetConstantBuffer(pConstantBuffer);

		// Activation de la texture
		ID3DX11EffectShaderResourceVariable* skyTexture;
		skyTexture = pEffet->GetVariableByName("textureEntree")->AsShaderResource();
		skyTexture->SetResource(pTexture);

		// Le sampler state
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);

		// **** Rendu de l'objet
		pPasse->Apply(0, pImmediateContext);

		pImmediateContext->DrawIndexed(GetNbSommets(), 0, 0);
	}

	int Skybox::GetNbSommets() const
	{
		return nbSommets;
	}

	const std::vector<CSommetBloc>& Skybox::GetSommets() const
	{
		return vectSommets;
	}

	const std::vector<unsigned int>& Skybox::GetIndexes() const
	{
		return pIndexesVect;
	}

	void Skybox::SetTexture(const std::wstring& texturePath)
	{
		pTexture = LoadTexture(texturePath)->GetD3DTexture();
	}

	void Skybox::MakeSkybox(const std::wstring& skybox)
	{
		//---------- Lecture de la heightmap ----------//
		std::ifstream monFlux(skybox);

		std::string ligne;
		int n = 0;

		if (monFlux)
		{

			while (std::getline(monFlux, ligne))
			{
				istringstream iss(ligne);
				vector<string> tokens;
				copy(istream_iterator<string>(iss), istream_iterator<string>(),
					back_inserter(tokens));

				if (n == 0) {
					nbSommets = std::stoi(tokens[0]);
				}
				else {
					float coeff = 4000.0f;
					CSommetBloc sommet = CSommetBloc(
						DirectX::XMFLOAT3(coeff*std::stof(tokens[0]),
							coeff*std::stof(tokens[1]),
							coeff*std::stof(tokens[2])
						),
						DirectX::XMFLOAT3(std::stof(tokens[5]),
							std::stof(tokens[6]),
							std::stof(tokens[7])
						),
						DirectX::XMFLOAT2(std::stof(tokens[3]) * textureRepeatX, std::stof(tokens[4]) * textureRepeatY
						)
					);

					vectSommets.push_back(sommet);
					pIndexesVect.push_back(n - 1);
				}

				n++;

				if (n>1&&(n-1) % 3 == 0) {
					auto temp = *(pIndexesVect.end()-1);
					*(pIndexesVect.end() - 1) = *(pIndexesVect.end() - 3);
					*(pIndexesVect.end() - 3) = temp;
				}
			}
		}
		else {
			std::cout << "ERREUR: Impossible d'ouvrir le fichier." << std::endl;
		}

		CSommetBloc* sommets = &vectSommets[0];
		unsigned int* pIndices = &pIndexesVect[0];

		//---------- End ----------//

		// Création du vertex buffer et copie des sommets
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CSommetBloc) * GetNbSommets();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = sommets;
		pVertexBuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

		// Création de l'index buffer et copie des indices
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * GetNbSommets();
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = pIndices;
		//InitData.pSysMem = index_bloc;

		pIndexBuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);
	}

	void Skybox::Load()
	{
		SetTexture(L"resources/Sky3.dds");
	}


	void Skybox::InitEffet()
	{
		// Compilation et chargement du vertex shader
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		// Création d'un tampon pour les constantes du VS
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

		// Pour l'effet
		ID3DBlob* pFXBlob = nullptr;

		DXEssayer(D3DCompileFromFile(L"resources/MiniPhongSkybox.fx", 0, 0, 0,
			"fx_5_0", 0, 0,
			&pFXBlob, nullptr),
			DXE_ERREURCREATION_FX);

		D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

		pFXBlob->Release();

		pTechnique = pEffet->GetTechniqueByIndex(0);
		pPasse = pTechnique->GetPassByIndex(0);

		// Créer l'organisation des sommets pour le VS de notre effet
		D3DX11_PASS_SHADER_DESC effectVSDesc;
		pPasse->GetVertexShaderDesc(&effectVSDesc);

		D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2);

		const void* vsCodePtr = effectVSDesc2.pBytecode;
		const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

		pVertexLayout = nullptr;
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout,
			CSommetBloc::numElements,
			vsCodePtr,
			vsCodeLen,
			&pVertexLayout),
			DXE_CREATIONLAYOUT);

		// Initialisation des paramètres de sampling de la texture
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		// Création de l’état de sampling
		pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
	}
}