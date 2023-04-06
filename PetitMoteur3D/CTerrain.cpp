#include "stdafx.h"
#include "CTerrain.h"
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

	void filterPente(vector<int>& data, const int width, const int height) {
		int i = 0;
		int x = 0;
		int y = 0;
		while (i != data.size()) {
			if (x % width == 0) {
				x = 0;
			}
			data[i] *= x * 5.0f / width;

			++x;
			++i;
		}
	}

	void filterBord(vector<int>& data, const int width, const int height) {
		int i = 0;
		int x = 0;
		int y = -1;
		while (y <= 0.20f * height) {
			if (x % width == 0) {
				x = 0;
				++y;
			}

			//data[i] += -150.0f / (0.20 * height) * y + 150;
			data[i] += 5333.33f * y * y / (height * height) - 2066.67f * y / height + 200.0f;

			++x;
			++i;
		}

		i = height * width -1;
		x = 0;
		y = height;
		while (y >= 0.80f * height) {
			if (x % width == 0) {
				x = 0;
				--y;
			}

			//data[i] += 150.0f / (0.20 * height) * y + (1.0 - 10.0/2.0) * 150;
			data[i] += 5333.33f * y * y / (height * height) - 8600.0f * y / height + 3466.67f;

			++x;
			--i;
		}
	}

	void filter(vector<int>& data, const int width, const int height) {
		filterPente(data, width, height);
		filterBord(data, width, height);
	}


	//  FONCTION : CBlocEffet1, constructeur paramètré 
	//  BUT :	Constructeur d'une classe de bloc avec effet voir 6.5
	//  PARAMÈTRES:		
	//		dx, dy, dz:	dimension en x, y, et z
	//		pDispositif: pointeur sur notre objet dispositif
	CTerrain::CTerrain(const wstring& heightmap) : CObjet3D()
		, pVertexBuffer{ nullptr }
		, pIndexBuffer{ nullptr }
		, pConstantBuffer{ nullptr }
		, pEffet{ nullptr }
		, pTechnique{ nullptr }
		, pVertexLayout{ nullptr }
		, vectSommets{}
		, pForeground{ nullptr }
		, pSampleState{ nullptr }
	{
		// Création de l'object 3D
		MakeTerrain(heightmap);

		// Initialisation de l'effet
		InitEffet();
	}

	CTerrain::~CTerrain()
	{
		DXRelacher(pVertexBuffer);
		DXRelacher(pIndexBuffer);
		DXRelacher(pConstantBuffer);
		DXRelacher(pEffet);
		DXRelacher(pVertexLayout);
	}

	void CTerrain::Draw(const CCamera& camera, const CLight& light, const CTransform& transform)
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
		ID3DX11EffectShaderResourceVariable* foreground;
		foreground = pEffet->GetVariableByName("textureForeground") -> AsShaderResource();
		foreground->SetResource(pForeground);

		ID3DX11EffectShaderResourceVariable* background;
		background = pEffet->GetVariableByName("textureBackground")->AsShaderResource();
		background->SetResource(pBackground);

		ID3DX11EffectShaderResourceVariable* ground;
		ground = pEffet->GetVariableByName("textureGround")->AsShaderResource();
		ground->SetResource(pGround);

		// Le sampler state
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);

		// **** Rendu de l'objet
		pPasse->Apply(0, pImmediateContext);

		pImmediateContext->DrawIndexed(nbPolygons*3, 0, 0);
	}

	int CTerrain::GetSizeX() const
	{
		return sizeX;
	}

	int CTerrain::GetSizeY() const
	{
		return sizeY;
	}

	int CTerrain::GetNbPolygons() const
	{
		return nbPolygons;
	}

	int CTerrain::GetNbSommets() const
	{
		return nbSommets;
	}

	const std::vector<CSommetBloc>& CTerrain::GetSommets() const
	{
		return vectSommets;
	}

	const std::vector<unsigned int>& CTerrain::GetIndexes() const
	{
		return pIndexesVect;
	}

	void CTerrain::SetForeground(const std::wstring& texturePath)
	{
		pForeground = LoadTexture(texturePath)->GetD3DTexture();
	}

	void CTerrain::SetBackground(const std::wstring& texturePath)
	{
		pBackground = LoadTexture(texturePath)->GetD3DTexture();
	}

	void CTerrain::SetGround(const std::wstring& texturePath)
	{
		pGround = LoadTexture(texturePath)->GetD3DTexture();
	}

	void CTerrain::MakeTerrain(const std::wstring& heightmap)
	{
		//---------- Lecture de la heightmap ----------//
		std::ifstream monFlux(heightmap);

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
					nbPolygons = std::stoi(tokens[1]);
					sizeX = std::stoi(tokens[2]);
					sizeY = std::stoi(tokens[3]);
				}
				else if (n > 0 && n < nbSommets + 1) {

					CSommetBloc sommet = CSommetBloc(
						DirectX::XMFLOAT3(std::stof(tokens[0]),
							std::stof(tokens[2]) + 1250,
							std::stof(tokens[1])
						),
						DirectX::XMFLOAT3(std::stof(tokens[3]),
							std::stof(tokens[4]),
							1*std::stof(tokens[5])
						),
						DirectX::XMFLOAT2(textureRepeat * std::stof(tokens[0]) / sizeX,
							textureRepeat * std::stof(tokens[1]) / sizeY
						)
					);

					vectSommets.push_back(sommet);
				}
				else if (n >= nbSommets + 1 && n < nbSommets + 1 + (nbPolygons)) {
					pIndexesVect.push_back(std::stoi(tokens[0]));
					pIndexesVect.push_back(std::stoi(tokens[1]));
					pIndexesVect.push_back(std::stoi(tokens[2]));
				}

				n++;
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

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(CSommetBloc) * nbSommets;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = sommets;
		pVertexBuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

		// Création de l'index buffer et copie des indices
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(unsigned int) * (nbPolygons) * 3;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = pIndices;
		//InitData.pSysMem = index_bloc;

		pIndexBuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);
	}

	void CTerrain::InitEffet()
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

		DXEssayer(D3DCompileFromFile(L"resources/MiniPhong.fx", 0, 0, 0,
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

	

} // namespace PM3D

