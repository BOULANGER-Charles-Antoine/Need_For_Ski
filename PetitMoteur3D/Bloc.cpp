#include "stdafx.h"
#include "Bloc.h"
#include "Sommet.h"
#include "util.h"
#include "resource.h"

using namespace DirectX;

namespace PM3D
{
	struct ShadersParams
	{
		DirectX::XMMATRIX matWorldViewProj;	// la matrice totale 
		DirectX::XMMATRIX matWorld;			// matrice de transformation dans le monde 
		DirectX::XMVECTOR vDirLum; 			// la direction de la lumiere
		DirectX::XMVECTOR vCamera; 			// la position de la caméra
		DirectX::XMVECTOR vAEcl; 			// la valeur ambiante de l'éclairage
		DirectX::XMVECTOR vAMat; 			// la valeur ambiante du matériau
		DirectX::XMVECTOR vDEcl; 			// la valeur diffuse de l'éclairage 
		DirectX::XMVECTOR vDMat; 			// la valeur diffuse du matériau 
	};

	//  FONCTION : CBloc, constructeur paramètré 
	//  BUT :	Constructeur d'une classe de bloc
	//  PARAMÈTRES:
	//		dx, dy, dz:	dimension en x, y, et z
	//		pDispositif: pointeur sur notre objet dispositif
	CBloc::CBloc(const float dx, const float dy, const float dz) : CObjet3D()
	{
		// Les points
		DirectX::XMFLOAT3 point[8] =
		{
			DirectX::XMFLOAT3(-dx / 2, dy / 2, -dz / 2),
			DirectX::XMFLOAT3(dx / 2, dy / 2, -dz / 2),
			DirectX::XMFLOAT3(dx / 2, -dy / 2, -dz / 2),
			DirectX::XMFLOAT3(-dx / 2, -dy / 2, -dz / 2),
			DirectX::XMFLOAT3(-dx / 2, dy / 2, dz / 2),
			DirectX::XMFLOAT3(-dx / 2, -dy / 2, dz / 2),
			DirectX::XMFLOAT3(dx / 2, -dy / 2, dz / 2),
			DirectX::XMFLOAT3(dx / 2, dy / 2, dz / 2)
		};

		// Calculer les normales
		const DirectX::XMFLOAT3 n0(0.0f, 0.0f, -1.0f); // devant
		const DirectX::XMFLOAT3 n1(0.0f, 0.0f, 1.0f); // arrière
		const DirectX::XMFLOAT3 n2(0.0f, -1.0f, 0.0f); // dessous
		const DirectX::XMFLOAT3 n3(0.0f, 1.0f, 0.0f); // dessus
		const DirectX::XMFLOAT3 n4(-1.0f, 0.0f, 0.0f); // face gauche
		const DirectX::XMFLOAT3 n5(1.0f, 0.0f, 0.0f); // face droite

		CSommetBloc sommets[24] =
		{
			// Le devant du bloc
			CSommetBloc(point[0], n0, DirectX::XMFLOAT2(0.0f, 0.0f)),
			CSommetBloc(point[1], n0, DirectX::XMFLOAT2(1.0f, 0.0f)),
			CSommetBloc(point[2], n0, DirectX::XMFLOAT2(1.0f, 1.0f)),
			CSommetBloc(point[3], n0, DirectX::XMFLOAT2(0.0f, 1.0f)),

			// L'arrière du bloc
			CSommetBloc(point[4], n1, DirectX::XMFLOAT2(0.0f, 1.0f)),
			CSommetBloc(point[5], n1, DirectX::XMFLOAT2(0.0f, 0.0f)),
			CSommetBloc(point[6], n1, DirectX::XMFLOAT2(1.0f, 0.0f)),
			CSommetBloc(point[7], n1, DirectX::XMFLOAT2(1.0f, 1.0f)),

			// Le dessous du bloc
			CSommetBloc(point[3], n2, DirectX::XMFLOAT2(0.0f, 0.0f)),
			CSommetBloc(point[2], n2, DirectX::XMFLOAT2(1.0f, 0.0f)),
			CSommetBloc(point[6], n2, DirectX::XMFLOAT2(1.0f, 1.0f)),
			CSommetBloc(point[5], n2, DirectX::XMFLOAT2(0.0f, 1.0f)),

			// Le dessus du bloc
			CSommetBloc(point[0], n3, DirectX::XMFLOAT2(0.0f, 1.0f)),
			CSommetBloc(point[4], n3, DirectX::XMFLOAT2(0.0f, 0.0f)),
			CSommetBloc(point[7], n3, DirectX::XMFLOAT2(1.0f, 0.0f)),
			CSommetBloc(point[1], n3, DirectX::XMFLOAT2(1.0f, 1.0f)),

			// La face gauche
			CSommetBloc(point[0], n4, DirectX::XMFLOAT2(0.0f, 0.0f)),
			CSommetBloc(point[3], n4, DirectX::XMFLOAT2(1.0f, 0.0f)),
			CSommetBloc(point[5], n4, DirectX::XMFLOAT2(1.0f, 1.0f)),
			CSommetBloc(point[4], n4, DirectX::XMFLOAT2(0.0f, 1.0f)),

			// La face droite
			CSommetBloc(point[1], n5, DirectX::XMFLOAT2(0.0f, 0.0f)),
			CSommetBloc(point[7], n5, DirectX::XMFLOAT2(1.0f, 0.0f)),
			CSommetBloc(point[6], n5, DirectX::XMFLOAT2(1.0f, 1.0f)),
			CSommetBloc(point[2], n5, DirectX::XMFLOAT2(0.0f, 1.0f))
		};

		// Création du vertex buffer et copie des sommets
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
		pVertexBuffer = nullptr;

		PM3D::DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

		// Création de l'index buffer et copie des indices
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(index_bloc);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = index_bloc;
		pIndexBuffer = nullptr;

		PM3D::DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);

		// Inititalisation des shaders
		InitShaders();
	}

	CBloc::~CBloc()
	{
		PM3D::DXRelacher(pVertexBuffer);
		PM3D::DXRelacher(pIndexBuffer);
		PM3D::DXRelacher(pConstantBuffer);
		PM3D::DXRelacher(pEffet);
		PM3D::DXRelacher(pVertexLayout);
	}

	void CBloc::Draw(const CCamera& camera, const CLight& light, const CTransform& transform)
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
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);

		// Initialiser et sélectionner les «constantes» de l'effet
		ShadersParams sp;
		DirectX::XMMATRIX viewProj = camera.GetMatViewProj();
		DirectX::XMMATRIX matWorld = transform.GetWorldMatrix();

		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		sp.matWorld = XMMatrixTranspose(matWorld);
		
		sp.vCamera = camera.GetTransform().GetPositionVector();
		sp.vDirLum = light.GetLightDirection(transform);
		sp.vAEcl = light.GetAmbient();
		sp.vDEcl = light.GetDiffuse();
		sp.vAMat = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
		sp.vDMat = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

		ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");  // Nous n'avons qu'un seul CBuffer
		pCB->SetConstantBuffer(pConstantBuffer);

		// **** Rendu de l'objet
		pPasse->Apply(0, pImmediateContext);

		pImmediateContext->DrawIndexed(ARRAYSIZE(index_bloc), 0, 0);
	}

	void CBloc::InitShaders()
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

		PM3D::DXEssayer(D3DCompileFromFile(L"resources/MiniPhong.fx", 0, 0, 0,
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
		PM3D::DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout,
			CSommetBloc::numElements,
			vsCodePtr,
			vsCodeLen,
			&pVertexLayout),
			DXE_CREATIONLAYOUT);
	}
}