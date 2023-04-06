#pragma once
#include "Scene.h"
#include "DIManipulateur.h"
#include "GestionnaireDeChargeur.h"
#include "PhysxEngine.h"

#include "Light.hpp"
#include "Obstacle.h"
#include "GraphicObject.h"

#include "Arbre.h"
#include "Roche.h"
#include "Poteau.h"
#include "Cabine.h"
#include "Chalet.h"
#include "Barriere.h"
#include "Tremplin.h"
#include "Tunnel.h"
#include "BonhommeNeige.h"
#include "Luge.h"
#include "Arrivee.h"
#include "Pingouin.h"
#include "GameManager.h"


#include <string>
#include <memory>

#include "UIContainer.h"

class MainScene final: public Scene
{
	//static constexpr const DirectX::XMFLOAT3 PLAYER_START{ -250, 50, 0 };
	static constexpr const DirectX::XMFLOAT3 PLAYER_START{ -20, 5.0f, 0 };		//DON'T TOUCH

	static CTransform PlayerInitTransform() noexcept
	{
		CTransform transform{};
		transform.SetPosition(PLAYER_START);
		transform.LookTo(DirectX::XMFLOAT3{ -1, 0, 0 });

		return transform;
	}

	std::shared_ptr<CFollowCamera> tpsCamera;
	std::shared_ptr<CFollowCamera> fpsCamera;
	std::shared_ptr<CFreeCamera> freeCamera;

	std::shared_ptr<CLight> globalLight;

	std::unique_ptr<UIContainer> playContainer = nullptr;
	std::unique_ptr<UIContainer> pauseContainer = nullptr;
	std::unique_ptr<UIResolution> resContainer = nullptr;
	std::unique_ptr<UISnow> snowContainer = nullptr;
	std::unique_ptr<UICredits> creditsContainer = nullptr;

	bool snow = true;

public:
	std::shared_ptr<CLight> GetSceneLight() const { return globalLight; }

	void Reset() 
	{
		Start();
	}

	bool GetSnow() const { return snow; }
	void SetSnow(bool snow) { this->snow = snow; }

protected:
	std::shared_ptr<Player> InitPlayer() override
	{
		std::shared_ptr pPlayer = std::make_shared<Player>();
		return pPlayer;
	}
	void InitObjects() override
	{
		//----- Terrain -----//
		InitTerrain();
		InitArbres();
		InitRoches();
		InitPoteaux();
		InitCables();
		InitCabines();
		InitChalets();
		InitBarrieres();
		InitTremplins();
		InitTunnels();
		InitBonhommesNeige();
		InitLuges();
		InitArrivee();
		InitPingouins();

		//----- Objets graphiques -----//
		AddObjectToScene(new HeightmapObject(
			L"resources/output.txt",
			L"resources/Snow_lowPoly.dds",
			L"resources/Rock_lowPoly.dds",
			L"resources/Dirt_Snow.dds"
		));
		AddObjectToScene(new SkyboxObject(L"resources/Sphere.txt"));
	}
	void InitOthers() override
	{
		//Camera
		tpsCamera = std::make_shared<TPSCamera>();
		fpsCamera = std::make_shared<FPSCamera>();
		freeCamera = std::make_shared<CFreeCamera>();

		//Light
		globalLight = std::shared_ptr<CLight>(new CDirectionnalLight(
			DirectX::XMVectorSet(1000, 500, 500, 1.0f),
			DirectX::XMVectorSet(0, 1, 0, 0),
			DirectX::XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f),
			DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f),
			DirectX::XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f)
		));

		//----- UI -----//
		playContainer = std::make_unique<UIPlay>();
		pauseContainer = std::make_unique<UIPause>();
		resContainer = std::make_unique<UIResolution>();
		snowContainer = std::make_unique<UISnow>();
		creditsContainer = std::make_unique<UICredits>();
	}

	void OnStart() override
	{
		GameManager::GetInstance().StartLevel();

		GetPlayer().lock()->GetTransform() = PlayerInitTransform();
		SetCamera(tpsCamera);
		SetLight(globalLight);

		snow = true;
	}
	void OnUpdate() override
	{
		const PM3D::CDIManipulateur& input = PM3D::CDIManipulateur::GetInstance();
		GameManager& gm = GameManager::GetInstance();

		//----- Input -----//
		if (input.ToucheEnfoncee(DIK_R)) {
			Reset();
		}
		if (input.ToucheEnfoncee(DIK_3))
		{
			std::weak_ptr<CCamera> currentCamera = GetCamera();
			freeCamera->GetTransform() = currentCamera.lock()->GetTransform();
			SetCamera(freeCamera);
		}
		if (input.ToucheEnfoncee(DIK_2))
		{
			switchCamera();
		}
		if (input.ToucheEnfoncee(DIK_P) || input.ToucheEnfoncee(DIK_ESCAPE))
		{
			gm.SetPause(!gm.IsPaused());
			setMenu("PauseMenu");
		}

		//----- Play Update -----//
		if (!gm.IsPaused())
		{
			playContainer->Update();
		}
		//----- Pause Update -----//
		else
		{
			snowContainer->Update();
			if (currentMenu == "PauseMenu")
			{
				pauseContainer->Update();
			}
			else if (currentMenu == "OptionsMenu")
			{
				resContainer->Update();
			}
			else if (currentMenu == "CreditsMenu")
			{
				creditsContainer->Update();
			}
		}

		snowContainer->Update();
	}
	void UIRender() const override
	{
		std::shared_ptr<CCamera> camera = GetCamera().lock();
		std::shared_ptr<CLight> light = GetLight().lock();

		//----- Play Render -----//
		if (!GameManager::GetInstance().IsPaused())
		{
			playContainer->Draw(*camera, *light, CTransform{});
			if (snow) snowContainer->Draw(*camera, *light, CTransform{});
		}
		//----- Pause Render -----//
		else
		{
			if (currentMenu == "PauseMenu")
			{
				pauseContainer->Draw(*camera, *light, CTransform{});
			}
			else if(currentMenu == "OptionsMenu")
			{
				resContainer->Draw(*camera, *light, CTransform{});
			}
			else if (currentMenu == "CreditsMenu")
			{
				creditsContainer->Draw(*camera, *light, CTransform{});
			}
		}
	}
	void OnClose() override 
	{
		GameManager::GetInstance().EndLevel();
	}

private:
	void switchCamera() noexcept
	{
		//FIXME

		if (GetCamera().lock() == tpsCamera)
		{
			SetCamera(fpsCamera);
		}
		else
		{
			SetCamera(tpsCamera);
		}
	}

#pragma region Objects Init
	void InitTerrain()
	{
		PM3D::CParametresChargement params;
		params.NomChemin = "./modeles/terrain/";
		params.bMainGauche = true;
		params.NomFichier = "terrainFinal.obj";

		AddObjectToScene(new TerrainObject(params));
	}
	void InitArbres() {
		PM3D::CParametresChargement paramsArbres;
		paramsArbres.NomChemin = "./modeles/arbre/";
		paramsArbres.bMainGauche = true;
		paramsArbres.NomFichier = "arbre.obj";

		AddObjectToScene(new Arbre("arbre0", paramsArbres,
			12.938f, -7.1692f, -2.4903f,
			90, 0.214, 0,
			1, 1, 1));

		AddObjectToScene(new Arbre("arbre1", paramsArbres,
			13.653f, -9.1878f, -2.4903f,
			84.4f, 0, 0,
			1.147f, 1.147f, 1.147f));

		AddObjectToScene(new Arbre("arbre2", paramsArbres,
			13.653f, 6.9059f, -2.4903f,
			87.2f, -0.713f, -5.59f,
			1.082f, 1.082f, 1.082f));

		AddObjectToScene(new Arbre("arbre3", paramsArbres,
			15.5f, 5.9f, -2.9f,
			95.6f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f));

		AddObjectToScene(new Arbre("arbre4", paramsArbres,
			32.716f, 8.0f, -5.8f,
			90.0f, 6.27f, -15.5f,
			1.079f, 1.079f, 1.079f));

		AddObjectToScene(new Arbre("arbre5", paramsArbres,
			39.65f, 2.23f, -7.1494f,
			92.8f, 5.65f, 16.6f,
			0.9f, 0.9f, 0.9f));

		AddObjectToScene(new Arbre("arbre6", paramsArbres,
			52.9f, 9.7f, -9.45f,
			94.3f, 4.65f, 32.8f,
			1.f, 1.f, 1.f));

		AddObjectToScene(new Arbre("arbre7", paramsArbres,
			55.697f, 8.4665f, -10.012f,
			92.1f, 5.73f, 33.4f,
			1.225f, 1.225f, 1.225f));

		AddObjectToScene(new Arbre("arbre8", paramsArbres,
			54.5f, 5.7f, -10.01f,
			91.7f, 5.17f, 10.8f,
			1.044f, 1.044f, 1.044f));

		AddObjectToScene(new Arbre("arbre9", paramsArbres,
			52.921f, 7.46f, -9.5579f,
			95.9f, 4.25f, 23.f,
			0.9f, 0.9f, 0.9f));

		AddObjectToScene(new Arbre("arbre10", paramsArbres,
			52.935f, -9.8354f, -9.3808f,
			85.3f, 0.f, -34.2f,
			1.044f, 1.044f, 1.044f));


		AddObjectToScene(new Arbre("arbre11", paramsArbres,
			55.677f, -8.1978f, -9.9f,
			84.1f, 2.98f, 11.9f,
			1.224f, 1.224f, 1.224f));

		AddObjectToScene(new Arbre("arbre12", paramsArbres,
			54.535f, -5.7634f, -9.7f,
			86.2f, 1.83f, 12.3f,
			1.f, 1.f, 1.f));

		AddObjectToScene(new Arbre("arbre13", paramsArbres,
			97.883f, 6.6318f, -17.482f,
			95.5f, 0.f, 193.f,
			1.f, 1.f, 1.f));

		AddObjectToScene(new Arbre("arbre14", paramsArbres,
			99.549f, -14.635f, -17.787f,
			92.2f, 1.41f, -196.f,
			1.326f, 1.326f, 1.326f));

		AddObjectToScene(new Arbre("arbre15", paramsArbres,
			-52.86f, -12.061f, 9.5538f,
			84.4f, 0.0f, 0.f,
			1.147f, 1.147f, 1.147f));

		AddObjectToScene(new Arbre("arbre16", paramsArbres,
			-52.86f, -9.7342f, 9.3264f,
			84.4f, 0.0f, 103.f,
			0.9f, 0.9f, 0.9f));

		AddObjectToScene(new Arbre("arbre17", paramsArbres,
			68.5f, -3.3486f, -12.221f,
			87.4f, -14.7f, 205.f,
			0.9f, 0.9f, 0.9f));

		AddObjectToScene(new Arbre("arbre18", paramsArbres,
			65.33f, -4.3291f, -11.787f,
			91.7f, 5.17f, 192.f,
			1.044f, 1.044f, 1.044f));

		AddObjectToScene(new Arbre("arbre19", paramsArbres,
			65.742f, -7.1059f, -11.813f,
			86.2f, -0.437f, 216.f,
			1.225f, 1.225f, 1.225f));

		AddObjectToScene(new Arbre("arbre20", paramsArbres,
			81.346f, 1.2454f, -14.618f,
			86.2f, 0.0f, 216.f,
			1.225f, 1.225f, 1.225f));

		AddObjectToScene(new Arbre("arbre21", paramsArbres,
			79.565f, 3.3482f, -14.275f,
			89.f, 1.0f, 204.f,
			1.f, 1.0f, 1.0f));

		AddObjectToScene(new Arbre("arbre22", paramsArbres,
			82.824f, 3.0677f, -14.904f,
			94.7f, -1.25f, 204.f,
			0.9f, 0.9f, 0.9f));

		AddObjectToScene(new Arbre("arbre23", paramsArbres,
			83.618f, 0.f, -15.f,
			83.f, 2.0f, 237.f,
			1.065f, 1.065f, 1.065f));

		AddObjectToScene(new Arbre("arbre24", paramsArbres,
			100.f, 5.23f, -17.766f,
			87.2f, 0.0f, -198.f,
			1.218f, 1.218f, 1.218f));

		AddObjectToScene(new Arbre("arbre25", paramsArbres,
			129.26f, -1.60f, -22.86f,
			87.f, 0.7f, -198.f,
			1.218f, 1.218f, 1.218f));

		AddObjectToScene(new Arbre("arbre26", paramsArbres,
			132.68f, -3.3638f, -23.52f,
			89.1f, -2.81f, -257.f,
			0.963f, 0.963f, 0.963f));

		AddObjectToScene(new Arbre("arbre27", paramsArbres,
			132.02f, -1.1468f, -23.388f,
			90.f, -2.86f, -288.f,
			0.963f));

		AddObjectToScene(new Arbre("arbre28", paramsArbres,
			158.02f, 0.f, -27.971f,
			92.9f, 0.6f, -377.f,
			0.963f, 0.963f, 0.963f));

		AddObjectToScene(new Arbre("arbre29", paramsArbres,
			155.81f, -1.34f, -27.498f,
			92.8f, -0.884f, -347.f,
			0.963f, 0.963f, 0.963f));

		//AddObjectToScene(new Arbre("arbre", paramsArbres,
		//	.f, .f, .f,
		//	.f, .f, .f,
		//	.f, .f, .f));

		AddObjectToScene(new Arbre("arbre30", paramsArbres,
			157.54f, 2.093f, -27.811f,
			90.7f, -2.85f, -288.f,
			1.218f, 1.218f, 1.218f));

		//AddObjectToScene(new Arbre("arbre", paramsArbres,
		//	.f, .f, .f,
		//	.f, .f, .f,
		//	.f));


		AddObjectToScene(new Arbre("arbre31", paramsArbres,
			157.15f, -10.099f, -27.927f,
			92.9f, 0.f, -5.25f,
			1.218f));

		AddObjectToScene(new Arbre("arbre32", paramsArbres,
			153.42f, -9.1755f, -27.324f,
			91.5f, 2.54f, -64.2f,
			0.963f));

		AddObjectToScene(new Arbre("arbre33", paramsArbres,
			154.57f, -11.171f, -27.32f,
			90.f, 2.94f, -94.6f,
			0.963f));

		AddObjectToScene(new Arbre("arbre34", paramsArbres,
			163.69f, -0.626f, -29.049f,
			92.8f, 1.02f, -385.f,
			1.218f));

		AddObjectToScene(new Arbre("arbre35", paramsArbres,
			160.49f, 1.4947f, -28.558f,
			90.5f, 2.89f, -444.f,
			0.963f));

		AddObjectToScene(new Arbre("arbre36", paramsArbres,
			160.9f, -0.77f, -28.486f,
			89.f, 2.76f, -474.f,
			0.963f));

		AddObjectToScene(new Arbre("arbre37", paramsArbres,
			736.92f, -0.77047f, -130.15f,
			89.f, 2.76f, -474.f,
			0.983f));

		AddObjectToScene(new Arbre("arbre38", paramsArbres,
			736.51f, 1.4947f, -130.22f,
			90.5f, 2.89f, -444.f,
			1.f));

		AddObjectToScene(new Arbre("arbre39", paramsArbres,
			739.71f, 2.4534f, -130.71f,
			92.8f, 1.02f, -385.f,
			1.218f));

		AddObjectToScene(new Arbre("arbre40", paramsArbres,
			730.59f, -11.171f, -128.98f,
			90.f, 2.94f, -94.6f,
			0.963f));

		AddObjectToScene(new Arbre("arbre41", paramsArbres,
			729.44f, -9.1755f, -128.99f,
			91.5f, 2.54f, -64.2f,
			0.963f));

		AddObjectToScene(new Arbre("arbre42", paramsArbres,
			733.17f, -10.f, -129.59f,
			92.9f, 0.f, -5.25f,
			1.218f));

		AddObjectToScene(new Arbre("arbre43", paramsArbres,
			733.55f, 2.093f, -129.48f,
			90.7f, -2.85f, -288.f,
			1.218f));

		AddObjectToScene(new Arbre("arbre44", paramsArbres,
			731.83f, 0.15f, -129.16f,
			92.8f, -0.884f, -347.f,
			0.963f));
		AddObjectToScene(new Arbre("arbre45", paramsArbres,
			734.03f, -0.6548f, -129.64f,
			92.9f, 0.653f, -377.f,
			0.963f));

		AddObjectToScene(new Arbre("arbre46", paramsArbres,
			708.04f, -1.1468f, -125.05f,
			90.7f, -2.86f, -288.f,
			1.f));

		AddObjectToScene(new Arbre("arbre47", paramsArbres,
			708.7f, -3.3638f, -125.18f,
			89.1f, -2.81f, -257.f,
			.963f));

		AddObjectToScene(new Arbre("arbre48", paramsArbres,
			705.28f, -1.6036f, -124.53f,
			87.2f, -0.713f, -198.f,
			1.218f));

		AddObjectToScene(new Arbre("arbre49", paramsArbres,
			675.57f, 5.23f, -119.43f,
			87.2f, -0.713f, -198.f,
			1.218f));

		AddObjectToScene(new Arbre("arbre50", paramsArbres,
			659.63f, 0.f, -116.67f,
			83.f, 1.56f, 237.f,
			1.065f));

		AddObjectToScene(new Arbre("arbre51", paramsArbres,
			658.84f, 3.0677f, -116.57f,
			94.7f, -1.25f, 204.f,
			0.997f));

		AddObjectToScene(new Arbre("arbre52", paramsArbres,
			655.58f, 3.3482f, -115.94f,
			89.f, 0957.f, 204.f,
			1.026f));

		AddObjectToScene(new Arbre("arbre53", paramsArbres,
			657.36f, 1.2454f, -116.28f,
			86.2f, -0.437f, 216.f,
			1.225f));

		AddObjectToScene(new Arbre("arbre54", paramsArbres,
			641.76f, -7.1059f, -113.48f,
			86.2f, -0.437f, 216.f,
			1.225f));

		AddObjectToScene(new Arbre("arbre55", paramsArbres,
			641.35f, -4.3291f, -113.45f,
			91.7f, 5.17f, 192.f,
			1.044f));

		AddObjectToScene(new Arbre("arbre56", paramsArbres,
			644.52f, -3.3486f, -113.89f,
			87.4f, -14.7f, 205.f,
			0.890f));

		AddObjectToScene(new Arbre("arbre57", paramsArbres,
			523.16f, -9.7342f, -92.338f,
			84.4f, 0.f, 103.f,
			0.934f));

		AddObjectToScene(new Arbre("arbre58", paramsArbres,
			523.16f, -12.061f, -92.11f,
			84.4f, 0.f, 0.f,
			1.147f));

		AddObjectToScene(new Arbre("arbre59", paramsArbres,
			675.57f, -14.635f, -119.45f,
			92.2f, 1.14f, -196.f,
			1.326f));

		AddObjectToScene(new Arbre("arbre60", paramsArbres,
			673.9f, 6.6318f, -119.15f,
			95.6f, 0.f, -193.f,
			1.f));

		AddObjectToScene(new Arbre("arbre61", paramsArbres,
			630.55f, -5.7634f, -111.34f,
			86.2f, 1.83f, -12.3f,
			1.f));

		AddObjectToScene(new Arbre("arbre62", paramsArbres,
			631.69f, -8.1978f, -111.58f,
			84.1f, 2.98f, -11.9f,
			1.225f));

		AddObjectToScene(new Arbre("arbre63", paramsArbres,
			628.95f, -9.8354f, -111.05f,
			85.3f, -0.5f, -34.2f,
			1.044f));

		AddObjectToScene(new Arbre("arbre64", paramsArbres,
			628.94f, 7.4608f, -111.22f,
			95.9f, 4.25f, 23.f,
			.890f));

		AddObjectToScene(new Arbre("arbre65", paramsArbres,
			630.59f, 5.7065f, -111.68f,
			91.7f, 5.17f, 10.8f,
			1.044f));

		AddObjectToScene(new Arbre("arbre66", paramsArbres,
			631.71f, 8.4665f, -111.68f,
			92.1f, 5.73f, 33.4f,
			1.225f));

		AddObjectToScene(new Arbre("arbre67", paramsArbres,
			628.9f, 9.7039f, -111.13f,
			94.3f, 4.65f, 32.8f,
			1.f));

		AddObjectToScene(new Arbre("arbre68", paramsArbres,
			615.66f, 2.2301f, -108.81f,
			92.8f, 5.65f, 16.6f,
			.893f));

		AddObjectToScene(new Arbre("arbre69", paramsArbres,
			608.73f, 8.0336f, -107.4f,
			89.4f, 6.27f, -15.4f,
			1.073f));

		AddObjectToScene(new Arbre("arbre70", paramsArbres,
			591.52f, 5.8997f, -104.53f,
			95.6f, 0.f, 0.f,
			1.f));

		AddObjectToScene(new Arbre("arbre71", paramsArbres,
			589.67f, 6.9059f, -104.15f,
			87.2f, -0.7f, -5.59f,
			1.082f));

		AddObjectToScene(new Arbre("arbre72", paramsArbres,
			589.67f, -9.1878f, -104.14f,
			84.4f, 0.f, 0.f,
			1.147f));

		AddObjectToScene(new Arbre("arbre73", paramsArbres,
			588.95f, -7.1692f, -104.15f,
			90.f, 0.214f, 0.f,
			1.f));

		AddObjectToScene(new Arbre("arbre74", paramsArbres,
			1301.2f, -0.77f, -229.59f,
			89.f, 2.76f, -474.f,
			0.963f));

		AddObjectToScene(new Arbre("arbre75", paramsArbres,
			1300.8f, 1.4947f, -229.66f,
			90.5f, 2.89f, -444.f,
			0.963f));

		AddObjectToScene(new Arbre("arbre76", paramsArbres,
			1304.f, -0.62606f, -230.16f,
			92.8f, 1.02f, -385.f,
			1.218f));

		AddObjectToScene(new Arbre("arbre77", paramsArbres,
			1294.9f, -11.171f, -228.43f,
			90.f, 2.94f, -94.6f,
			0.963f));

		AddObjectToScene(new Arbre("arbre78", paramsArbres,
			1293.8f, -9.1755f, -228.43f,
			91.5f, 2.54f, -64.2f,
			0.963f));

		AddObjectToScene(new Arbre("arbre79", paramsArbres,
			1297.5f, -10.f, -229.f,
			92.9f, 0.04f, -5.25f,
			1.218f));

		AddObjectToScene(new Arbre("arbre80", paramsArbres,
			1297.9f, 2.093f, -228.92f,
			90.7f, -2.85f, -288.f,
			1.218f));

		AddObjectToScene(new Arbre("arbre81", paramsArbres,
			1296.1f, -1.34f, -228.61f,
			92.8f, -0.884f, -347.f,
			0.963f));

		AddObjectToScene(new Arbre("arbre82", paramsArbres,
			1298.3f, -0.65487f, -229.08f,
			92.8f, 0.653f, -377.f,
			0.963f));

		AddObjectToScene(new Arbre("arbre83", paramsArbres,
			1272.4f, -1.1468f, -224.5f,
			90.7f, -2.86f, -288.f,
			0.963f));

		AddObjectToScene(new Arbre("arbre84", paramsArbres,
			1273.f, -3.3638f, -224.63f,
			89.1f, -2.81f, -257.f,
			0.963f));

		AddObjectToScene(new Arbre("arbre85", paramsArbres,
			1269.6f, -1.6036f, -223.97f,
			87.2f, -.713f, -198.f,
			1.218f));

		AddObjectToScene(new Arbre("arbre86", paramsArbres,
			1239.9f, -6.7265f, -218.87f,
			87.2f, -0.713f, -198.f,
			1.110f));

		AddObjectToScene(new Arbre("arbre87", paramsArbres,
			1223.9f, -9.0602f, -216.12f,
			83.f, 1.56f, 237.f,
			1.065f));

		AddObjectToScene(new Arbre("arbre88", paramsArbres,
			1223.2f, -6.087f, -216.01f,
			94.7f, -1.25f, 204.f,
			0.997f));

		AddObjectToScene(new Arbre("arbre89", paramsArbres,
			1219.9f, -5.8066f, -215.38f,
			89.f, 0.957f, 204.f,
			1.050f));

		AddObjectToScene(new Arbre("arbre90", paramsArbres,
			1221.7f, -7.9094f, -215.72f,
			86.2f, -0.437f, 216.f,
			1.225f));

		AddObjectToScene(new Arbre("arbre91", paramsArbres,
			1206.1f, -7.1059f, -212.92f,
			86.2f, -0.437f, 216.f,
			1.225f));

		AddObjectToScene(new Arbre("arbre92", paramsArbres,
			1205.7f, -4.3291f, -212.89f,
			91.7f, 5.17f, 192.f,
			1.044f));

		AddObjectToScene(new Arbre("arbre93", paramsArbres,
			1208.8f, -3.3486f, -213.33f,
			87.4f, -14.7f, 205.f,
			0.890f));

		AddObjectToScene(new Arbre("arbre94", paramsArbres,
			1087.5f, -13.549f, -191.78f,
			84.4f, 0.7f, 103.f,
			0.934f));

		AddObjectToScene(new Arbre("arbre95", paramsArbres,
			1087.5f, -15.876f, -191.55f,
			84.4f, 0.f, 0.f,
			1.147f));

		AddObjectToScene(new Arbre("arbre96", paramsArbres,
			1239.9f, -9.8831f, -218.89f,
			92.2f, 1.41f, -196.f,
			1.326f));

		AddObjectToScene(new Arbre("arbre97", paramsArbres,
			1238.2f, -5.3247f, -218.59f,
			95.6f, 0.f, -193.f,
			1.f));

		AddObjectToScene(new Arbre("arbre98", paramsArbres,
			1194.9f, 6.4997f, -210.78f,
			86.2f, 1.83f, -12.3f,
			1.f));

		AddObjectToScene(new Arbre("arbre99", paramsArbres,
			1196.f, 4.0653f, -211.01f,
			84.1f, 2.98f, -11.9f,
			1.225f));

		AddObjectToScene(new Arbre("arbre100", paramsArbres,
			1193.3f, 2.4277f, -210.49f,
			85.5f, -0.439f, -34.2f,
			1.044f));

		AddObjectToScene(new Arbre("arbre101", paramsArbres,
			1193.3f, -6.6417f, -210.66f,
			95.9f, 4.25f, 23.f,
			0.890f));

		AddObjectToScene(new Arbre("arbre102", paramsArbres,
			1194.9f, -8.396f, -211.12f,
			91.7f, 5.17f, 10.8f,
			1.044f));

		AddObjectToScene(new Arbre("arbre103", paramsArbres,
			1196.f, -5.6361f, -211.12f,
			92.1f, 5.73f, 33.4f,
			1.225f));

		AddObjectToScene(new Arbre("arbre104", paramsArbres,
			1193.2f, -4.3987f, -210.57f,
			94.3f, 4.65f, 32.8f,
			1.f));

		AddObjectToScene(new Arbre("arbre105", paramsArbres,
			1180.f, 2.2301f, -208.26f,
			92.8f, 5.65f, 16.6f,
			0.893f));

		AddObjectToScene(new Arbre("arbre106", paramsArbres,
			1173.f, 8.0336f, -206.88f,
			89.4f, 6.27f, -15.4f,
			1.079f));

		AddObjectToScene(new Arbre("arbre107", paramsArbres,
			1155.8f, 5.8997f, -203.98f,
			95.6f, 0.f, 0.f,
			1.f));

		AddObjectToScene(new Arbre("arbre108", paramsArbres,
			1154.f, 6.9059f, -203.6f,
			87.2f, -0.713f, -5.59f,
			1.082f));

		AddObjectToScene(new Arbre("arbre109", paramsArbres,
			1154.f, -6.6585f, -203.6f,
			84.4f, 0.f, .0f,
			1.147f));

		AddObjectToScene(new Arbre("arbre110", paramsArbres,
			1153.3f, -4.6399f, -203.6f,
			90.f, 0.214f, 0.f,
			1.0f));

		AddObjectToScene(new Arbre("arbre111", paramsArbres,
			733.33f, -7.1692f, -129.61f,
			90.f, 0.214f, 0.f,
			1.f));

		AddObjectToScene(new Arbre("arbre112", paramsArbres,
			734.05f, -9.1878f, -129.61f,
			84.4f, 0.f, 0.f,
			1.147f));

		AddObjectToScene(new Arbre("arbre113", paramsArbres,
			734.05f, 6.9059f, -129.61f,
			87.2f, -.713f, -5.59f,
			1.082f));

		AddObjectToScene(new Arbre("arbre114", paramsArbres,
			735.89f, 5.8997f, -129.99f,
			95.6f, 0.f, 0.f,
			1.f));

		AddObjectToScene(new Arbre("arbre115", paramsArbres,
			753.11f, 8.0336f, -132.9f,
			89.4f, 6.27f, -15.4f,
			1.079f));

		AddObjectToScene(new Arbre("arbre116", paramsArbres,
			760.04f, 2.2301f, -134.27f,
			92.8f, 5.65f, 16.6f,
			0.893f));

		AddObjectToScene(new Arbre("arbre117", paramsArbres,
			773.27f, 11.989f, -136.59f,
			94.3f, 4.65f, 32.8f,
			1.f));

		AddObjectToScene(new Arbre("arbre118", paramsArbres,
			776.09f, 10.751f, -137.12f,
			92.1f, 5.73f, 33.4f,
			1.225f));

		AddObjectToScene(new Arbre("arbre119", paramsArbres,
			774.97f, 7.9913f, -137.13f,
			91.7f, 5.17f, 10.8f,
			1.044f));

		AddObjectToScene(new Arbre("arbre120", paramsArbres,
			773.31f, 9.7456f, -136.68f,
			95.9f, 4.25f, 23.f,
			0.890f));

		AddObjectToScene(new Arbre("arbre121", paramsArbres,
			773.33f, -9.8354f, -136.5f,
			85.3f, -0.439f, -34.2f,
			1.044f));

		AddObjectToScene(new Arbre("arbre122", paramsArbres,
			776.07f, -8.1978f, -137.04f,
			84.1f, 2.98f, -11.9f,
			1.225f));

		AddObjectToScene(new Arbre("arbre123", paramsArbres,
			774.93f, -5.7634f, -136.79f,
			86.2f, 1.83f, -12.3f,
			1.f));

		AddObjectToScene(new Arbre("arbre124", paramsArbres,
			818.28f, 6.6318f, -144.6f,
			95.6f, 0.f, -193.f,
			1.f));

		AddObjectToScene(new Arbre("arbre125", paramsArbres,
			819.94f, -9.5689f, -144.91f,
			92.2f, 1.41f, -196.f,
			1.326f));

		AddObjectToScene(new Arbre("arbre126", paramsArbres,
			667.53f, -12.06f, -117.57f,
			84.4f, 0.f, 0.f,
			1.147f));

		AddObjectToScene(new Arbre("arbre127", paramsArbres,
			667.53f, -9.7342f, -117.79f,
			84.4f, 0.f, 103.f,
			0.934f));

		AddObjectToScene(new Arbre("arbre128", paramsArbres,
			788.9f, 0.72422f, -139.34f,
			87.4f, -14.7f, 205.f,
			0.890f));

		AddObjectToScene(new Arbre("arbre129", paramsArbres,
			785.72f, -0.25631f, -138.91f,
			91.7f, 5.17f, 192.f,
			1.044f));

		AddObjectToScene(new Arbre("arbre130", paramsArbres,
			786.10f, -3.0331f, -138.93f,
			86.2f, -0.437f, 216.f,
			1.225f));

		AddObjectToScene(new Arbre("arbre131", paramsArbres,
			801.74f, 1.2454f, -141.74f,
			86.2f, -0.437f, 216.f,
			1.225f));

		AddObjectToScene(new Arbre("arbre132", paramsArbres,
			799.96f, 3.3482f, -141.39f,
			89.f, 0.1f, 204.f,
			1.026f));

		AddObjectToScene(new Arbre("arbre133", paramsArbres,
			803.22f, 3.0677f, -142.02f,
			94.7f, -1.25f, 204.f,
			0.988f));

		//------ALEXOS---------*
		AddObjectToScene(new Arbre("arbre134", paramsArbres,
			804.01f, .09f, -142.13f,
			83.f, 1.56f, 237.f,
			1.065f));
		AddObjectToScene(new Arbre("arbre135", paramsArbres,
			819.94f, 5.23f, -144.89f,
			87.2f, -.71f, -198.f,
			1.218f));
		AddObjectToScene(new Arbre("arbre136", paramsArbres,
			849.66f, -1.6f, -149.98f,
			87.2f, -.71f, -198.f,
			1.218f));
		AddObjectToScene(new Arbre("arbre137", paramsArbres,
			853.08f, -3.36f, -150.64f,
			89.1f, -2.81f, -257.f,
			.963f));
		AddObjectToScene(new Arbre("arbre138", paramsArbres,
			852.42f, -1.15f, -150.51f,
			90.7f, -2.86f, -288.f,
			.963f));
		AddObjectToScene(new Arbre("arbre139", paramsArbres,
			878.41f, 2.64f, -155.09f,
			92.9f, .65f, -377.f,
			.963f));
		AddObjectToScene(new Arbre("arbre140", paramsArbres,
			876.f, 1.96f, -154.62f,
			92.8f, -.88f, -347.f,
			.963f));
		AddObjectToScene(new Arbre("arbre141", paramsArbres,
			877.93f, 5.39f, -154.93f,
			90.7f, -2.85f, -288.f,
			1.218f));
		AddObjectToScene(new Arbre("arbre142", paramsArbres,
			877.55f, -7.65f, -155.05f,
			92.9f, .04f, -5.25f,
			1.218f));
		AddObjectToScene(new Arbre("arbre143", paramsArbres,
			873.82f, -6.72f, -154.44f,
			91.5f, 2.54f, -64.2f,
			.963f));
		AddObjectToScene(new Arbre("arbre145", paramsArbres,
			874.97f, -8.72f, -154.44f,
			90.f, 2.94f, -94.6f,
			.963f));
		AddObjectToScene(new Arbre("arbre144", paramsArbres,
			884.09f, 2.67f, -156.17f,
			92.8f, 1.02f, -385.f,
			1.218f));
		AddObjectToScene(new Arbre("arbre146", paramsArbres,
			880.88f, 4.79f, -155.68f,
			90.5f, 2.89f, -444.f,
			.963f));
		AddObjectToScene(new Arbre("arbre147", paramsArbres,
			881.29f, 2.53f, -155.6f,
			89.f, 2.76f, -474.f,
			.963f));
		AddObjectToScene(new Arbre("arbre148", paramsArbres,
			325.49f, 5.71f, -57.97f,
			91.7f, 5.17f, 10.8f,
			1.044f));
		AddObjectToScene(new Arbre("arbre149", paramsArbres,
			323.84f, 7.46f, -57.51f,
			95.9f, 4.25f, 23.f,
			.89f));
		AddObjectToScene(new Arbre("arbre150", paramsArbres,
			323.86f, -9.84f, -57.34f,
			85.3f, -.439f, -34.2f,
			1.044f));
		AddObjectToScene(new Arbre("arbre151", paramsArbres,
			326.6f, -8.2f, -57.87f,
			84.1f, 2.98f, -11.9f,
			1.225f));
		AddObjectToScene(new Arbre("arbre152", paramsArbres,
			325.46f, -5.76f, -57.63f,
			86.2f, 1.83f, -12.3f,
			1.f));
		AddObjectToScene(new Arbre("arbre153", paramsArbres,
			368.8f, 6.63f, -65.47f,
			95.6f, 0.f, -193.f,
			1.f));
		AddObjectToScene(new Arbre("arbre154", paramsArbres,
			370.47f, -14.64f, -65.74f,
			92.2f, 1.42f, -196.f,
			1.326f));
		AddObjectToScene(new Arbre("arbre155", paramsArbres,
			218.06f, -12.06f, -38.4f,
			84.4f, 0.f, 0.f,
			1.147f));
		AddObjectToScene(new Arbre("arbre156", paramsArbres,
			218.06f, -9.73f, -38.63f,
			84.4f, 0.f, 103.f,
			0.934f));
		AddObjectToScene(new Arbre("arbre157", paramsArbres,
			339.42f, -3.35f, -60.18f,
			87.4f, -14.7f, 205.f,
			0.890f));
		AddObjectToScene(new Arbre("arbre158", paramsArbres,
			336.25f, -4.33f, -59.74f,
			91.7f, 5.17f, 192.f,
			1.044f));
		AddObjectToScene(new Arbre("arbre159", paramsArbres,
			336.66f, -7.11f, -59.77f,
			86.2f, -0.44f, 216.f,
			1.225f));
		AddObjectToScene(new Arbre("arbre160", paramsArbres,
			352.27f, 1.25f, -62.572f,
			86.2f, -0.44f, 216.f,
			1.225f));
		AddObjectToScene(new Arbre("arbre161", paramsArbres,
			350.79f, 3.35f, -62.23f,
			89.f, 0.96f, 204.f,
			1.026f));
		AddObjectToScene(new Arbre("arbre162", paramsArbres,
			353.74f, 3.07f, -62.86f,
			94.7f, -1.25f, 204.f,
			0.997f));
		AddObjectToScene(new Arbre("arbre163", paramsArbres,
			354.54f, -0.09f, -62.94f,
			83.f, 1.56f, 237.f,
			1.065f));
		AddObjectToScene(new Arbre("arbre164", paramsArbres,
			370.47f, 5.23f, -65.72f,
			87.2f, -0.71f, -198.f,
			1.218f));
		AddObjectToScene(new Arbre("arbre165", paramsArbres,
			400.19f, -1.6f, -70.82f,
			87.2f, -0.71f, -198.f,
			1.218f));
		AddObjectToScene(new Arbre("arbre166", paramsArbres,
			403.6f, -3.36f, -71.48f,
			89.1f, -2.81f, -257.f,
			0.963f));
		AddObjectToScene(new Arbre("arbre167", paramsArbres,
			402.94f, -1.15f, -71.343f,
			90.7f, -2.86f, -288.f,
			0.963f));
		AddObjectToScene(new Arbre("arbre168", paramsArbres,
			409.46f, 3.24f, -72.29f,
			92.9f, 0.65f, -377.f,
			0.963f));
		AddObjectToScene(new Arbre("arbre169", paramsArbres,
			407.25f, 2.55f, -71.82f,
			92.8f, -0.88f, -347.f,
			0.963f));
		AddObjectToScene(new Arbre("arbre170", paramsArbres,
			421.91f, 2.09f, -74.69f,
			90.7f, -2.85f, -288.f,
			1.218f));
		AddObjectToScene(new Arbre("arbre171", paramsArbres,
			408.59f, -6.21f, -72.25f,
			92.9f, 0.04f, -5.25f,
			1.218f));
		AddObjectToScene(new Arbre("arbre172", paramsArbres,
			404.86f, -5.28f, -71.64f,
			91.5f, 2.54f, -64.2f,
			0.963f));
		AddObjectToScene(new Arbre("arbre173", paramsArbres,
			406.01f, -7.28f, -71.64f,
			90.f, 2.94f, -94.f,
			0.963f));
		AddObjectToScene(new Arbre("arbre174", paramsArbres,
			412.34f, 3.12f, -72.8f,
			89.f, 2.76f, -474.f,
			0.963f, 0.963f, 0.963f));
		AddObjectToScene(new Arbre("arbre175", paramsArbres,
			411.93f, 5.39f, -72.88f,
			90.5f, 2.89f, -444.f,
			0.963f, 0.963f, 0.963f));

		AddObjectToScene(new Arbre("arbre176", paramsArbres,
			412.34f, 3.12f, -72.8f,
			89.f, 2.76f, -474.f,
			0.963f, 0.963f, 0.963f));

	}
	void InitRoches() {
		PM3D::CParametresChargement paramsRoches;
		paramsRoches.NomChemin = "./modeles/roche/";
		paramsRoches.bMainGauche = true;
		paramsRoches.NomFichier = "roche.obj";

		AddObjectToScene(new Roche("roche0", paramsRoches,
			12.62f, 4.11f, -1.82f,
			90.f, 0.f, 0.f,
			1.f));
		AddObjectToScene(new Roche("roche1", paramsRoches,
			12.6f, 2.94f, -1.93f,
			90.f, 0.f, -74.f,
			.521f));
		AddObjectToScene(new Roche("roche2", paramsRoches,
			20.23f, -3.23f, -3.26f,
			57.7f, 1.15f, -15.f,
			1.f));
		AddObjectToScene(new Roche("roche3", paramsRoches,
			37.68f, 1.37f, -6.34f,
			90.f, 0.f, 70.4f,
			1.f));
		AddObjectToScene(new Roche("roche4", paramsRoches,
			52.5f, -4.9f, -8.89f,
			90.f, 0.f, 143.f,
			1.f));
		AddObjectToScene(new Roche("roche5", paramsRoches,
			54.6f, -2.49f, -9.69f,
			90.f, 0.f, 39.8f,
			1.742f));
		AddObjectToScene(new Roche("roche6", paramsRoches,
			77.52f, 1.09f, -13.58f,
			90.f, 0.f, 136.f,
			1.742f));
		AddObjectToScene(new Roche("roche7", paramsRoches,
			79.42f, -.71f, -14.13f,
			90.f, 0.f, 182.f,
			2.132f, 1.752f, 2.1f));
		AddObjectToScene(new Roche("roche8", paramsRoches,
			77.52f, -.94f, -13.58f,
			90.f, 0.f, 136.f,
			.579f));
		AddObjectToScene(new Roche("roche9", paramsRoches,
			77.03f, -1.58f, -13.58f,
			90.f, 0.f, 253.f,
			.380f));
		AddObjectToScene(new Roche("roche10", paramsRoches,
			125.95f, -1.08f, -21.7f,
			90.f, 0.f, 125.f,
			1.357f, 1.108f, 1.336));
		AddObjectToScene(new Roche("roche11", paramsRoches,
			130.02f, -4.42f, -22.97f,
			90.f, 0.f, 176.f,
			.808f, .66f, 0.796f));
		AddObjectToScene(new Roche("roche12", paramsRoches,
			118.89f, -10.74f, -20.97f,
			90.f, 0.f, 125.f,
			1.357f, 1.108f, 1.336));
		AddObjectToScene(new Roche("roche13", paramsRoches,
			128.5f, -3.61f, -22.73f,
			90.f, 0.f, 125.f,
			.808f, .66f, 0.796f));
		AddObjectToScene(new Roche("roche14", paramsRoches,
			142.98f, -13.57f, -27.09f,
			90.f, 0.f, 125.f,
			2.577, 2.106f, 2.539f));
		AddObjectToScene(new Roche("roche15", paramsRoches,
			140.4f, -16.35f, -24.09f,
			90.f, 0.f, 125.f,
			2.577f, 2.106f, 2.539f));
		AddObjectToScene(new Roche("roche16", paramsRoches,
			144.69f, -16.35f, -24.09f,
			90.f, 0.f, 57.4f,
			4.146f, 3.387f, 4.084f));
		AddObjectToScene(new Roche("roche17", paramsRoches,
			147.76f, -14.f, -24.89f,
			90.f, 0.f, 98.6f,
			4.146f, 3.387f, 4.084f));
		AddObjectToScene(new Roche("roche18", paramsRoches,
			723.78f, -14.f, -126.55f,
			90.f, 0.f, 98.6f,
			4.146f, 3.387f, 4.084f));
		AddObjectToScene(new Roche("roche19", paramsRoches,
			720.71f, -16.35f, -125.75f,
			90.f, 0.f, 57.4f,
			4.146f, 3.387f, 4.084f));
		AddObjectToScene(new Roche("roche20", paramsRoches,
			716.42f, -16.35f, -125.75f,
			90.f, 0.f, 125.f,
			2.577f, 2.106f, 2.539f));
		AddObjectToScene(new Roche("roche21", paramsRoches,
			718.99f, -13.572f, -125.75f,
			90.f, 0.f, 125.f,
			2.577f, 2.106f, 2.539f));
		AddObjectToScene(new Roche("roche22", paramsRoches,
			704.52f, -3.61f, -124.4f,
			90.f, 0.f, 125.f,
			.808f, .66f, 0.796f));
		AddObjectToScene(new Roche("roche23", paramsRoches,
			694.91f, -10.74f, -122.63f,
			90.f, 0.f, 125.f,
			1.357f, 1.102f, 1.336f));
		AddObjectToScene(new Roche("roche24", paramsRoches,
			706.03f, -4.42f, -124.63f,
			90.f, 0.f, 176.f,
			.808f, .66f, 0.796f));
		AddObjectToScene(new Roche("roche25", paramsRoches,
			701.96f, -1.08f, -123.37f,
			90.f, 0.f, 125.f,
			1.357f, 1.102f, 1.336f));
		AddObjectToScene(new Roche("roche26", paramsRoches,
			653.05f, -1.58f, -115.24f,
			90.f, 0.f, 253.f,
			.38f));
		AddObjectToScene(new Roche("roche27", paramsRoches,
			653.54f, -.94f, -115.24f,
			90.f, 0.f, 136.f,
			.579f));
		AddObjectToScene(new Roche("roche28", paramsRoches,
			655.43f, -.71f, -115.79f,
			90.f, 0.f, -115.79f,
			2.132f, 1.742f, 2.1f));
		AddObjectToScene(new Roche("roche29", paramsRoches,
			653.54f, 1.09f, -115.24f,
			90.f, 0.f, 136.f,
			1.742f));
		AddObjectToScene(new Roche("roche30", paramsRoches,
			630.61f, -2.49f, -111.35f,
			90.f, 0.f, 39.8f,
			1.742f));
		AddObjectToScene(new Roche("roche31", paramsRoches,
			628.52f, -4.9f, -110.55f,
			90.f, 0.f, 143.f,
			1.f));
		AddObjectToScene(new Roche("roche32", paramsRoches,
			613.69f, 1.37f, -108.f,
			90.f, 0.f, 70.4f,
			1.f));
		AddObjectToScene(new Roche("roche33", paramsRoches,
			596.24f, -3.23f, -104.92f,
			57.7f, 1.15f, -15.f,
			1.f));
		AddObjectToScene(new Roche("roche34", paramsRoches,
			588.62f, 2.94f, -103.59f,
			90.f, 0.f, -74.f,
			.521f));
		AddObjectToScene(new Roche("roche35", paramsRoches,
			588.64f, 4.11f, -103.48f,
			90.f, 0.f, 0.f,
			1.f));
		AddObjectToScene(new Roche("roche36", paramsRoches,
			1288.1f, -14.f, -225.99f,
			90.f, -14.f, -225.99f,
			4.146f, 3.387f, 4.084));
		AddObjectToScene(new Roche("roche37", paramsRoches,
			1285.f, -16.35f, -255.19f,
			90.f, 0.f, 57.4f,
			4.146f, 3.387f, 4.084));
		AddObjectToScene(new Roche("roche38", paramsRoches,
			1280.7f, -16.35f, -225.19f,
			90.f, 0.f, 125.f,
			2.577f, 2.106f, 2.539f));
		AddObjectToScene(new Roche("roche39", paramsRoches,
			1283.3f, -13.57f, -225.19f,
			90.f, 0.f, 125.f,
			2.577f, 2.106f, 2.539f));
		AddObjectToScene(new Roche("roche40", paramsRoches,
			1268.8f, -3.61f, -223.84f,
			90.f, 0.f, 125.f,
			.808f, .66f, .796f));
		AddObjectToScene(new Roche("roche41", paramsRoches,
			1259.2f, -10.74f, -222.08f,
			90.f, 0.f, 125.f,
			1.357f, 1.108f, 1.336f));
		AddObjectToScene(new Roche("roche42", paramsRoches,
			1270.3f, -4.42f, -224.07f,
			90.f, 0.f, 176.f,
			.808f, .66f, .796f));
		AddObjectToScene(new Roche("roche43", paramsRoches,
			1266.3f, -1.08f, -222.81f,
			90.f, 0.f, 125.f,
			1.357f, 1.108f, 1.336f));
		AddObjectToScene(new Roche("roche44", paramsRoches,
			1217.4f, -10.73f, -214.68f,
			90.f, 0.f, 253.f,
			.38f));
		AddObjectToScene(new Roche("roche45", paramsRoches,
			1217.9f, -10.09f, -214.68f,
			90.f, 0.f, 136.f,
			.579f));
		AddObjectToScene(new Roche("roche46", paramsRoches,
			1217.7f, -9.87f, -215.24f,
			90.f, 0.f, 182.f,
			2.132f, 1.742f, 2.1f));
		AddObjectToScene(new Roche("roche47", paramsRoches,
			1217.9f, -8.07f, -214.68f,
			90.f, 0.f, 136.f,
			1.742f));
		AddObjectToScene(new Roche("roche48", paramsRoches,
			1194.9f, 9.78f, -210.8f,
			90.f, 0.f, 39.8f,
			1.742f));
		AddObjectToScene(new Roche("roche49", paramsRoches,
			1192.8f, 7.37f, -210.f,
			90.f, 0.f, 143.f,
			1.f));
		AddObjectToScene(new Roche("roche50", paramsRoches,
			1178.f, 1.37f, -207.45f,
			90.f, 0.f, 70.4f,
			1.f));
		AddObjectToScene(new Roche("roche51", paramsRoches,
			1160.6f, 5.2f, -204.36f,
			57.7f, 1.15f, 15.f,
			1.f));
		AddObjectToScene(new Roche("roche52", paramsRoches,
			1152.9f, 2.94f, -203.03f,
			90.f, 0.f, -74.f,
			.521f));
		AddObjectToScene(new Roche("roche53", paramsRoches,
			1152.9f, 4.11f, -202.93f,
			90.f, 0.f, 0.f,
			1.f));
		AddObjectToScene(new Roche("roche54", paramsRoches,
			733.01f, 4.11f, -128.94f,
			90.f, 0.f, 0.f,
			1.f));
		AddObjectToScene(new Roche("roche55", paramsRoches,
			732.99f, 2.94f, -129.05f,
			90.f, 0.f, -74.f,
			.521f));
		AddObjectToScene(new Roche("roche56", paramsRoches,
			740.62f, -.45f, -130.37f,
			57.7f, 1.15f, -15.f,
			1.f));
		AddObjectToScene(new Roche("roche57", paramsRoches,
			758.07f, 1.38f, -133.46f,
			90.f, 0.f, 70.4f,
			1.f));
		AddObjectToScene(new Roche("roche58", paramsRoches,
			772.9f, -4.896f, -136.01f,
			90.f, 0.f, 143.f,
			1.f));
		AddObjectToScene(new Roche("roche59", paramsRoches,
			774.99f, 4.47f, -136.81f,
			90.f, 0.f, -136.81f,
			1.742f));
		AddObjectToScene(new Roche("roche60", paramsRoches,
			797.92f, 1.085f, -140.7f,
			90.f, 0.f, 136.f,
			1.742f));
		AddObjectToScene(new Roche("roche61", paramsRoches,
			799.81f, -.71f, -141.25f,
			90.f, -.71f, -141.25f,
			2.132f, 1.742f, 2.1f));
		AddObjectToScene(new Roche("roche62", paramsRoches,
			797.92f, -.94f, -140.7f,
			90.f, 0.f, 136.f,
			.579f));
		AddObjectToScene(new Roche("roche63", paramsRoches,
			797.43f, -1.58f, -140.7f,
			90.f, 0.f, 253.f,
			.380f));
		AddObjectToScene(new Roche("roche64", paramsRoches,
			846.34f, -1.075f, -148.82f,
			90.f, 0.f, 125.f,
			1.357f, 1.108f, 1.336f));
		AddObjectToScene(new Roche("roche65", paramsRoches,
			850.41f, -4.42f, -150.09f,
			90.f, 0.f, 176.f,
			.808f, .66f, .796f));
		AddObjectToScene(new Roche("roche66", paramsRoches,
			839.29f, -10.74f, -148.09f,
			90.f, 0.f, 125.f,
			1.357f, 1.108f, 1.336f));
		AddObjectToScene(new Roche("roche67", paramsRoches,
			848.9f, -3.61f, -149.85f,
			90.f, 0.f, 125.f,
			.808f, .66f, .796f));
		AddObjectToScene(new Roche("roche72", paramsRoches,
			283.54f, 4.11f, -49.77f,
			90.f, 0.f, 0.f,
			1.f));
		AddObjectToScene(new Roche("roche73", paramsRoches,
			283.52f, 2.94f, -49.88f,
			90.f, 0.f, -74.f,
			.521f));
		AddObjectToScene(new Roche("roche74", paramsRoches,
			308.6f, 1.37f, -54.295f,
			90.f, 0.f, 70.4f,
			1.f));
		AddObjectToScene(new Roche("roche75", paramsRoches,
			348.44f, -.94f, -61.53f,
			90.f, 0.f, 136.f,
			.579f));
		AddObjectToScene(new Roche("roche76", paramsRoches,
			347.95f, -1.58f, -61.53f,
			90.f, 0.f, 253.f,
			.382f));
		AddObjectToScene(new Roche("roche77", paramsRoches,
			389.81f, -10.74f, -68.925f,
			90.f, 0.f, 125.f,
			1.357f, 1.108f, 1.336f));
		AddObjectToScene(new Roche("roche78", paramsRoches,
			413.9f, -13.57f, -72.04f,
			90.f, 0.f, 125.f,
			2.577f, 2.106f, 2.539f));
		AddObjectToScene(new Roche("roche79", paramsRoches,
			411.32f, -16.35f, -72.04f,
			90.f, 0.f, 125.f,
			2.577f, 2.106f, 2.539f));

		AddObjectToScene(new Roche("roche70", paramsRoches,
			464.98, 19.526, -81.384,
			112, 47.5, 130,
			3.257, 2.661, 3.208));

		AddObjectToScene(new Roche("roche71", paramsRoches,
			466.58, 13.435, -81.458,
			92.7, -5.29, 161,
			3.275, 2.675, 3.226));

		AddObjectToScene(new Roche("roche68", paramsRoches,
			459.76, 14.436, -80.772,
			95.3, -2.6, 124,
			3.924, 3.206, 3.865));

		AddObjectToScene(new Roche("roche69", paramsRoches,
			460.92, 7.2262, -80.784,
			86.9, -5.04, 220,
			0.985, 0.805, 0.971));

		AddObjectToScene(new Roche("roche80", paramsRoches,
			466.47, 7.9179, -81.365,
			94.6, 3.76, 58.9,
			2.145, 1.752, 2.113));

		AddObjectToScene(new Roche("roche81", paramsRoches,
			462.78, 9.4168, 81.007,
			83.7, 147, 121,
			1.653, 1.350, 1.628));
	}
	void InitPoteaux() {
		PM3D::CParametresChargement params;
		params.NomChemin = "./modeles/poteau/";
		params.bMainGauche = true;
		params.NomFichier = "poteau.obj";

		AddObjectToScene(new Poteau("poteau0", params,
			21.381f, -15.229f, -1.8268f,
			90, 0.f, 0.f,
			1.f));

		AddObjectToScene(new Poteau("poteau1", params,
			288.33f, 34.622f, -37.031f,
			90.f, 0.f, 39.4f,
			1.f));

		AddObjectToScene(new Poteau("poteau2", params,
			73.f, -15.229f, -13.485f,
			90.f, 0.f, 0.f,
			1.f));

		AddObjectToScene(new Poteau("poteau3", params,
			311.77f, 67.618f, -29.281f,
			90.f, 0.f, 62.7f,
			1.f));

		AddObjectToScene(new Poteau("poteau4", params,
			-24.898f, -15.229f, 6.1317f,
			90.f, 0.f, .0f,
			1.f));

		AddObjectToScene(new Poteau("poteau5", params,
			-70.646f, -15.229f, 11.674f,
			90.f, 0.f, 0.f,
			1.f));

		AddObjectToScene(new Poteau("poteau6", params,
			129.1f, -15.229f, -23.364f,
			90.f, 0.f, 5.39f,
			1.f));

		AddObjectToScene(new Poteau("poteau7", params,
			185.57f, -2.537f, -33.4f,
			19.3f, 24.9f, 33.f,
			1.f));

		AddObjectToScene(new Poteau("poteau8", params,
			240.29f, 14.56f, -41.28f,
			90.f, 0.f, 23.5f,
			1.f));
	}
	void InitCables() {
		PM3D::CParametresChargement params;
		params.NomChemin = "./modeles/cables/";
		params.bMainGauche = true;
		params.NomFichier = "cables.obj";

		AddObjectToScene(new Obstacle("cables", params));
	}
	void InitCabines() {
		PM3D::CParametresChargement params;
		params.NomChemin = "./modeles/cabine/";
		params.bMainGauche = true;
		params.NomFichier = "cabine.obj";

		AddObjectToScene(new Cabine("cabine0", params,
			-12.713, -11.725, 15.399,
			90, -11.3, 180,
			1
		));

		AddObjectToScene(new Cabine("cabine1", params,
			-69.564, -11.725, 22.242,
			90, 0, 180,
			1
		));

		AddObjectToScene(new Cabine("cabine2", params,
			-41.456, -11.725, 18.252,
			90, -10.4, 180,
			1
		));

		AddObjectToScene(new Cabine("cabine3", params,
			45.255, -11.725, 4.0414,
			90, -4.42, 180,
			1
		));

		AddObjectToScene(new Cabine("cabine4", params,
			16, -11.725, 9.25,
			90, -7.3, 180,
			1
		));

		AddObjectToScene(new Cabine("cabine5", params,
			69.608, -11.725, -2.8278,
			90, -4.42, 180,
			1
		));

		AddObjectToScene(new Cabine("cabine6", params,
			98.392, -11.725, -8.0224,
			90, -11.6, 180,
			1
		));

		AddObjectToScene(new Cabine("cabine7", params,
			125.53, -11.725, -12.798,
			90, -3.19, 180,
			1
		));

		AddObjectToScene(new Cabine("cabine8", params,
			149, 3.786, -24.731,
			41.6, -20.1, 214,
			1
		));

		AddObjectToScene(new Cabine("cabine9", params,
			244.07, 19.972, -30.66,
			90, 1.13, 203,
			1
		));

		AddObjectToScene(new Cabine("cabine10", params,
			272.09, 30.145, -27.967,
			90, 3.51, 198
			, 1
		));

		AddObjectToScene(new Cabine("cabine11", params,
			294.53, 47.367, -24.517,
			90, 17.5, 235,
			1
		));

		AddObjectToScene(new Cabine("cabine12", params,
			312.57, 78.027, -18.726,
			89.3, -0.838, 250,
			1
		));

		AddObjectToScene(new Cabine("cabine13", params,
			208.16, 17.109, -29.754,
			-9.9, 13.1, 188,
			1
		));
	}
	void InitBarrieres() {
		PM3D::CParametresChargement params;
		params.NomChemin = "./modeles/barriere/";
		params.bMainGauche = true;
		params.NomFichier = "barriere.obj";

		AddObjectToScene(new Barriere("barriere1", params,
			26.246, -4.2398, -4.686,
			91.7, 6.36, 60.3,
			1));

		AddObjectToScene(new Barriere("barriere2", params,
			24.666, -5.7463, -4.526,
			91.7, 6.36, 29.6,
			1));

		AddObjectToScene(new Barriere("barriere3", params,
			14.669, -4.7134, -2.7347,
			103, 3.94, 49.3,
			1));

		AddObjectToScene(new Barriere("barriere4", params,
			589.96, 1.586, -104.25,
			95.4, 0.844, 102,
			1));

		AddObjectToScene(new Barriere("barriere5", params,
			590.69, -4.7134, -104.4,
			103, 3.94, 49.3,
			1));

		AddObjectToScene(new Barriere("barriere6", params,
			600.68, -5.7463, -106.19,
			91.7, 6.36, 29.6,
			1));

		AddObjectToScene(new Barriere("barriere7", params,
			602.26, -4.2398, -106.35,
			91.7, 6.36, 60.3,
			1));

		AddObjectToScene(new Barriere("barriere10", params,
			13.944, 1.586, -2.5809,
			95.4, 0.844, 102,
			1));

		AddObjectToScene(new Barriere("barriere11", params,
			1165, 3.9875, -205.63,
			96.1, -2.36, -234,
			1));

		AddObjectToScene(new Barriere("barriere12", params,
			1166.6, 2.2749, -205.79,
			96.1, -2.41, 157,
			1));

		AddObjectToScene(new Barriere("barriere13", params,
			297.17, -4.2398, -52.641,
			91.7, 6.36, 60.3,
			1));

		AddObjectToScene(new Barriere("barriere14", params,
			295.59, -5.7463, -52.48,
			91.7, 6.36, 29.6,
			1));

		AddObjectToScene(new Barriere("barriere15", params,
			285.59, -4.7134, -50.689,
			103, 3.94, 49.3,
			1));

		AddObjectToScene(new Barriere("barriere16", params,
			284.86, 1.586, -50.535,
			65.4, 0.844, 102,
			1));
	}
	void InitChalets() {
		PM3D::CParametresChargement params;
		params.NomChemin = "./modeles/chalet/";
		params.bMainGauche = true;
		params.NomFichier = "chalet.obj";

		AddObjectToScene(new Chalet("chalet0", params,
			38.401, 6.15, -6.7936,
			93.4, 9.07, 36.2,
			1));

		AddObjectToScene(new Chalet("chalet1", params,
			48.112, -8.8732, -8.4811,
			88.9, 9.65, -31.1,
			1));

		AddObjectToScene(new Chalet("chalet2", params,
			113.13, -9.1966, -19.865,
			78.2, 2.41, -78.6,
			1));

		AddObjectToScene(new Chalet("chalet3", params,
			689.15, -9.1966, -121.53,
			78.2, 2.41, -78.6,
			1));

		AddObjectToScene(new Chalet("chalet4", params,
			624.13, -8.8732, -110.15,
			88.9, 9.65, -31.1,
			1));

		AddObjectToScene(new Chalet("chalet5", params,
			614.42, 6.15, -108.46,
			93.4, 9.07, 36.2,
			1));

		AddObjectToScene(new Chalet("chalet6", params,
			1253.5, 1.0905, -220.97,
			92.2, 11.8, 371,
			1));

		AddObjectToScene(new Chalet("chalet7", params,
			1188.4, -8.8732, -209.59,
			88.9, 9.65, -31.1,
			1));

		AddObjectToScene(new Chalet("chalet8", params,
			1178.7, 6.15, -207.9,
			93.4, 9.07, 36.2,
			1));

		AddObjectToScene(new Chalet("chalet9", params,
			758.79, 6.15, -133.91,
			93.4, 9.07, 36.2,
			1));

		AddObjectToScene(new Chalet("chalet10", params,
			768.51, -8.8732, -135.6,
			88.9, 9.65, -31.1,
			1));

		AddObjectToScene(new Chalet("chalet11", params,
			833.53, -8.1039, -146.98,
			78.2, 2.41, -78.6,
			1));

		AddObjectToScene(new Chalet("chalet12", params,
			422.61, 29.381, -74.55,
			93.4, 1.1, 96.1,
			1));

		AddObjectToScene(new Chalet("chalet13", params,
			441.57, 16.904, -77.865,
			93.4, 9.07, 23.3,
			1));

		AddObjectToScene(new Chalet("chalet14", params,
			436.86, 28.154, -76.865,
			93.4, 4.66, 42.8,
			1));

	}
	void InitTremplins() {
		PM3D::CParametresChargement params;
		params.NomChemin = "./modeles/tremplin/";
		params.bMainGauche = true;
		params.NomFichier = "tremplin.obj";

		AddObjectToScene(new Tremplin("tremplin0", params,
			11.952, -1.3624, -2.1542,
			90, 0, 182,
			1));

		AddObjectToScene(new Tremplin("tremplin1", params,
			88.841, 5.7089, -15.703,
			90, 0, 182,
			1));

		AddObjectToScene(new Tremplin("tremplin2", params,
			177.47, 1.6478, -31.329,
			90, 0, 167,
			1));

		AddObjectToScene(new Tremplin("tremplin3", params,
			753.48, 1.6478, -132.99,
			90, 0, 167,
			1));

		AddObjectToScene(new Tremplin("tremplin4", params,
			664.86, 5.7089, -117.37,
			90, 0, 182,
			1));

		AddObjectToScene(new Tremplin("tremplin5", params,
			587.97, -1.3624, -103.82,
			90, 0, 182,
			1));

		AddObjectToScene(new Tremplin("tremplin6", params,
			1325.5, -4.809, -233.78,
			90, 0, 185,
			1));

		AddObjectToScene(new Tremplin("tremplin7", params,
			1229.2, -5.0575, -216.81,
			90, 0, 182,
			1));

		AddObjectToScene(new Tremplin("tremplin8", params,
			1152.3, -1.3624, -203.26,
			90, 0, 182,
			1));

		AddObjectToScene(new Tremplin("tremplin9", params,
			476.65, -3.0238, -84.203,
			90, 0, 182,
			1.245, 1.0, 3.856));
	}
	void InitTunnels() {
		/*PM3D::CParametresChargement params;
		params.NomChemin = "./modeles/tunnel/";
		params.bMainGauche = true;
		params.NomFichier = "tunnel.obj";*/

		/*	AddObjectToScene(new Tunnel("tunnel",
				292.21, 0.61163, -64.156,
				0, 0, 0,
				1));

			AddObjectToScene(new Tunnel("tunnel1",
				868.23, 0.61163, -165.82,
				0, 0, 0,
				1));

			AddObjectToScene(new Tunnel("tunnel2",
				1432.5, 0.61163, -265.26,
				0, 0, 0,
				1));*/

		CPointLight* firstLight = new CPointLight(
			XMVectorSet(874.1, -2.9135, -143.69, 1),
			XMVectorSet(0.5f, 0.3f, 0.4f, 1.0f),
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f),
			XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f)
		);

		AddObjectToScene(new Tunnel("tunnel", firstLight,
			874.01, 0.6, -154.12,
			0, 0, 0,
			40.346, 17.452, 19.447));

		CPointLight* secondLight = new CPointLight(
			XMVectorSet(874.1, -2.9135, -143.69, 1),
			XMVectorSet(0.5f, 0.3f, 0.4f, 1.0f),
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f),
			XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f)
		);

		AddObjectToScene(new Tunnel("tunnel1", secondLight,
			1436.4, -2.8937, -255.25,
			0, 0, 0,
			40.346, 17.452, 19.447));

		CPointLight* thirdLight = new CPointLight(
			XMVectorSet(874.1, -2.9135, -143.69, 1),
			XMVectorSet(0.5f, 0.3f, 0.4f, 1.0f),
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f),
			XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f)
		);

		AddObjectToScene(new Tunnel("tunnel2", thirdLight,
			299, -2.8937, -56.523,
			0, 0, 0,
			40.346, 17.452, 19.447));
	}
	void InitBonhommesNeige() {
		PM3D::CParametresChargement params;
		params.NomChemin = "./modeles/bonhommeNeige/";
		params.bMainGauche = true;
		params.NomFichier = "bonhommeNeige.obj";

		AddObjectToScene(new BonhommeNeige("bonhommeNeige0", params,
			118.42, 7.9062, 7.9977,
			99.4, -6.48, 1348,
			1));

		AddObjectToScene(new BonhommeNeige("bonhommeNeige1", params,
			105.01, -28.61, 13.142,
			79.6, 12.6, 76,
			1));

		AddObjectToScene(new BonhommeNeige("bonhommeNeige2", params,
			62.608, 4.3784, -10.786,
			82, -2.53, 1370,
			0.375));

		AddObjectToScene(new BonhommeNeige("bonhommeNeige3", params,
			638.62, 4.3784, -112.45,
			82, -2.53, 1370,
			0.375));

		AddObjectToScene(new BonhommeNeige("bonhommeNeige6", params,
			1202.9, 4.3784, -211.89,
			82, -2.53, 1370,
			0.375));
	}
	void InitLuges() {
		PM3D::CParametresChargement params;
		params.NomChemin = "./modeles/luge/";
		params.bMainGauche = true;
		params.NomFichier = "luge.obj";

		AddObjectToScene(new Luge("luge1", params,
			36.536, 2.6686, -5.6983,
			-5.5, 48.1, -142,
			1));

		AddObjectToScene(new Luge("luge2", params,
			62.72, 4.2127, -10.813,
			9.38, -4.95, -241,
			1));

		AddObjectToScene(new Luge("luge3", params,
			68.369, -2.2654, -11.251,
			19.2, 60.4, -224,
			1));

		AddObjectToScene(new Luge("luge4", params,
			644.39, -2.2654, -112.92,
			19.2, 60.4, -224,
			1));

		AddObjectToScene(new Luge("luge5", params,
			638.74, 4.2127, -112.48,
			9.38, -4.95, -241,
			1));

		AddObjectToScene(new Luge("luge6", params,
			612.55, 2.6686, -107.36,
			-5.5, 48.1, -142,
			1));

		AddObjectToScene(new Luge("luge7", params,
			1208.7, -2.2654, -212.36,
			19.2, 60.4, -224,
			1));

		AddObjectToScene(new Luge("luge8", params,
			1203, 4.2127, -211.92,
			9.38, -4.95, -241,
			1));

		AddObjectToScene(new Luge("luge9", params,
			1176.9, 2.6686, -206.81,
			-5.5, 48.1, -142,
			1));

		AddObjectToScene(new Luge("luge10", params,
			333.64, 4.2127, -58.767,
			9.38, -4.95, -241,
			1));
	}
	void InitArrivee() {

		AddObjectToScene(new Arrivee("arrivee",
			1714.9, -2.8937, -288.4,
			0, 0, 0,
			112.204, 137.701, 174.588));
	}
	void InitPingouins() {
		PM3D::CParametresChargement params;
		params.NomChemin = "./modeles/pingouin/";
		params.bMainGauche = true;
		params.NomFichier = "pingouin.obj";


		/*AddObjectToScene(new Pingouin(XMVectorSet(-15,5,0,1), XMVectorSet(-1, 0, 0,1)));*/

		float offset = 0.3;

		AddObjectToScene(new Pingouin("pingouin0", params,
			965.66, 0, -170.35 + offset,
			90, 0, 24.7,
			1));

		AddObjectToScene(new Pingouin("pingouin1", params,
			969.8, 3.1209, -171.02 + offset,
			90, 0, 3.14,
			1));

		AddObjectToScene(new Pingouin("pingouin2", params,
			964.99, 6.6876, -170.11 + offset,
			90, 0, -74.2,
			1));

		AddObjectToScene(new Pingouin("pingouin3", params,
			969.8, 10.254, -171.05 + offset,
			90, 0, 31.4,
			1));

		AddObjectToScene(new Pingouin("pingouin4", params,
			966.99, 13.487, -170.49 + offset,
			90, 0, 32,
			1));

		AddObjectToScene(new Pingouin("pingouin5", params,
			969.8, 17.165, -171.05 + offset,
			90, 0, -23.3,
			1));

		AddObjectToScene(new Pingouin("pingouin6", params,
			969.8, 22.403, -171.03 + offset,
			90, 0, -32.4,
			1));

		AddObjectToScene(new Pingouin("pingouin7", params,
			965.12, 20.62, -170.06 + offset,
			90, 0, 41.3,
			1));

		AddObjectToScene(new Pingouin("pingouin8", params,
			977.56, 21.066, -172.44 + offset,
			90, 0, 43.9,
			1));

		AddObjectToScene(new Pingouin("pingouin9", params,
			981.17, 12.238, -173.238 + offset,
			90, 0, -27.6,
			1));

		AddObjectToScene(new Pingouin("pingouin10", params,
			975.42, 16.474, -172.09 + offset,
			90, 0, -25.4,
			1));

		AddObjectToScene(new Pingouin("pingouin11", params,
			974.48, 12.238, -171.88 + offset,
			90, 0, 28.7,
			1));

		AddObjectToScene(new Pingouin("pingouin12", params,
			978.76, 8.3372, -172.62 + offset,
			90, 0, 0,
			1));

		AddObjectToScene(new Pingouin("pingouin13", params,
			975.29, 4.659, -172.04 + offset,
			90, 0, -28.1,
			1));

		AddObjectToScene(new Pingouin("pingouin14", params,
			971.14, -2.5859, -171.29 + offset,
			90, 0, -35.4,
			1));

		AddObjectToScene(new Pingouin("pingouin15", params,
			977.29, -0.13375, -172.34 + offset,
			90, 0, 34.3,
			1));

		AddObjectToScene(new Pingouin("pingouin16", params,
			976.89, -5.7067, -172.37 + offset,
			90, 0, 0,
			1));

		AddObjectToScene(new Pingouin("pingouin17", params,
			971.81, -8.8276, -171.44 + offset,
			90, 0, -20.40,
			1));

		AddObjectToScene(new Pingouin("pingouin18", params,
			955.36, -8.8276, -168.43 + offset,
			90, 0, -26.10,
			1));

		AddObjectToScene(new Pingouin("pingouin19", params,
			960.84, -5.7067, -169.49 + offset,
			90, 0, -90.5,
			1));

		AddObjectToScene(new Pingouin("pingouin20", params,
			952.82, -2.14, -168.05 + offset,
			90, 0, 20.9,
			1));

		AddObjectToScene(new Pingouin("pingouin21", params,
			960.44, 1.4267, -169.41 + offset,
			90, 0, -36.90,
			1));


		AddObjectToScene(new Pingouin("pingouin22", params,
			950.41, 4.569, -167.51 + offset,
			90, 0, -48.6,
			1));


		AddObjectToScene(new Pingouin("pingouin23", params,
			956.43, 9.9422, -168.73 + offset,
			90, 0, -61.4,
			1));


		AddObjectToScene(new Pingouin("pingouin24", params,
			954.29, -19.595, -168.32 + offset,
			90, 0, 32.3,
			1));

		AddObjectToScene(new Pingouin("pingouin25", params,
			959.37, 16.474, -169.28 + offset,
			90, 0, -305,
			1));

		AddObjectToScene(new Pingouin("pingouin26", params,
			962.18, 12.238, -169.69 + offset,
			90, 0, 23.9,
			1));

		AddObjectToScene(new Pingouin("pingouin27", params,
			992.41, 12.238, -175.16 + offset,
			90, 0, 16,
			1));

		AddObjectToScene(new Pingouin("pingouin28", params,
			986.66, 16.474, -174.1 + offset,
			90, 0, -28.9,
			1));
		AddObjectToScene(new Pingouin("pingouin29", params,
			986.66, 12.238, -174.17 + offset,
			90, 0, 31.5,
			1));

		AddObjectToScene(new Pingouin("pingouin30", params,
			984.38, 8.3372, -173.65 + offset,
			90, -0.573, -34.9,
			1));
		AddObjectToScene(new Pingouin("pingouin31", params,
			990.27, 4.659, -174.68 + offset,
			90, 0, 20.9,
			1));
		AddObjectToScene(new Pingouin("pingouin32", params,
			986.66, 1.4267, -174.15 + offset,
			90, 0, 43.7,
			1));
		AddObjectToScene(new Pingouin("pingouin33", params,
			983.04, -2.1, -173.4 + offset,
			90, 0, -29.5,
			1));
		AddObjectToScene(new Pingouin("pingouin34", params,
			986.12, -5.7067, -173.96 + offset,
			90, 0, -43.6,
			1));
		AddObjectToScene(new Pingouin("pingouin35", params,
			931.44, -8.8276, -173.08 + offset,
			90, 0, -32.9,
			1));

	}
#pragma endregion
};

class LoadingScreen : public Scene
{
	std::unique_ptr<UIContainer> ui = nullptr;

	std::shared_ptr<CCamera> cam;
	std::shared_ptr<CLight> light;

protected:
	std::shared_ptr<Player> InitPlayer() override { return std::shared_ptr<Player>(); }
	void InitOthers() override
	{
		cam = std::make_shared<CCamera>();
		light = std::make_shared<CPointLight>(
			DirectX::XMVectorSet(440, 280, 70, 1),
			DirectX::XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f),
			DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f),
			DirectX::XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f)
		);

		ui = std::make_unique<UILoading>();
	}

	void OnStart()
	{
		SetCamera(cam);
		SetLight(light);
	}
	void OnUpdate() override { ui->Update(); }
	void UIRender() const override 
	{
		ui->Draw(*cam, *light, CTransform{});
	}
};