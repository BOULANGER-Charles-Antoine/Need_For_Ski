#include "stdafx.h"
#include "UIContainer.h"
#include "MoteurWindows.h"
#include "GameManager.h"
#include <iomanip>
#include <iostream>

using namespace PM3D;
using namespace std;

UIContainer::~UIContainer()
{
	if (afficheur) delete afficheur;
}

void UIContainer::Draw(const CCamera& camera, const CLight& light, const CTransform& transform)
{
	if(afficheur) afficheur->Draw(camera, light, transform);
}

PM3D::CAfficheurSprite* UIContainer::GetAfficheur() noexcept
{
	return afficheur;
}

void UIPlay::Update()
{
	#pragma region setup
	if (afficheur) delete(afficheur);
	afficheur = new PM3D::CAfficheurSprite();
	// Setup de l'afficheurTexte
	PM3D::CAfficheurTexte::Init();
	const Gdiplus::FontFamily oFamily(L"Arial", nullptr);
	Gdiplus::Font* pPolice = new Gdiplus::Font(&oFamily, 18.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	#pragma endregion

	#pragma region vitesse
	afficheur->AjouterSprite("resources/IconeVitesse.dds", "IconeVitesse", 870, 750, 40, 40);

	PM3D::CAfficheurTexte* vitesseVal = new PM3D::CAfficheurTexte(38, 20, pPolice);
	auto& a = static_cast<physx::PxRigidDynamic&>(PM3D::CMoteurWindows::GetInstance().GetCurrentScene()->GetPlayer().lock()->GetActor());
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << a.getLinearVelocity().magnitude();
	std::string b = stream.str();
	std::wstring widestr = std::wstring(b.begin(), b.end());
	const wchar_t* widecstr = widestr.c_str();
	vitesseVal->Ecrire(widecstr);
	afficheur->AjouterSpriteTexte(vitesseVal->GetTextureView(), 930, 740);
	
	PM3D::CAfficheurTexte* vitesseUnits = new PM3D::CAfficheurTexte(100, 20, pPolice);
	vitesseUnits->Ecrire(L"km/h");
	afficheur->AjouterSpriteTexte(vitesseUnits->GetTextureView(), 968, 740);
	#pragma endregion

	#pragma region chrono
	afficheur->AjouterSprite("resources/IconeChrono.dds", "IconeChrono", 467, 50, 40, 40);

	PM3D::CAfficheurTexte* timer = new PM3D::CAfficheurTexte(80, 20, pPolice);
	double chrono = GameManager::GetInstance().GetChrono();
	std::stringstream timerstream;
	timerstream << std::fixed << std::setprecision(2) << chrono;
	std::string timerstr = timerstream.str();
	std::wstring timerwstr = std::wstring(timerstr.begin(), timerstr.end());
	const wchar_t* timerwcstr = timerwstr.c_str();
	timer->Ecrire(timerwcstr);
	afficheur->AjouterSpriteTexte(timer->GetTextureView(), 517, 40);
	#pragma endregion
	
	#pragma region score
	PM3D::CAfficheurTexte* score = new PM3D::CAfficheurTexte(200, 20, pPolice);
	std::string scorestr = "Score : " + std::to_string(GameManager::GetInstance().GetScore());
	std::wstring scorewstr = std::wstring(scorestr.begin(), scorestr.end());
	const wchar_t* scorewcstr = scorewstr.c_str();
	score->Ecrire(scorewcstr);
	afficheur->AjouterSpriteTexte(score->GetTextureView(), 20, 750);
	#pragma endregion

	#pragma region multiplicateur
	int combo = GameManager::GetInstance().GetCombo();

	// size
	const float minSizeX = 150.f;
	const float minSizeY = 150.f;

	const float maxSizeX = 100.f;
	const float maxSizeY = 100.f;

	// pos
	const float maxPosX = 447.f;
	const float maxPosY = 715.f;
	
	const float minPosX = 427.f;
	const float minPosY = 735.f;

	const int maxFrames = 75;
	const int minFrames = 1;

	static int lastCombo = 0;

	static int nbFramesRestantes = maxFrames;
	if (combo != lastCombo) {
		nbFramesRestantes = maxFrames;
	}
	float x = ((minPosX - maxPosX) / (maxFrames - minFrames)) * nbFramesRestantes + minPosX - (minPosX - maxPosX) / (maxFrames - minFrames) * maxFrames;
	float y = ((minPosY - maxPosY) / (maxFrames - minFrames)) * nbFramesRestantes + minPosY - (minPosY - maxPosY) / (maxFrames - minFrames) * maxFrames;

	float dx = ((minSizeX - maxSizeX) / (maxFrames - minFrames)) * nbFramesRestantes + minSizeX - (minSizeX - maxSizeX) / (maxFrames - minFrames) * maxFrames;
	float dy = ((minSizeY - maxSizeY) / (maxFrames - minFrames)) * nbFramesRestantes + minSizeY - (minSizeY - maxSizeY) / (maxFrames - minFrames) * maxFrames;

	//x < minPosX ? x = minPosX : x;
	//y < minPosY ? y = minPosY : y;
	//
	//dx > maxSizeX ? dx = maxSizeX : dx;
	//dy > maxSizeY ? dy = maxSizeY : dy;

	std::string path = "resources/combos/" + std::to_string(combo) + ".dds";

	afficheur->AjouterSprite(path, "multiplier", x, y, dx, dy);

	lastCombo = combo;
	nbFramesRestantes == minFrames ? nbFramesRestantes = minFrames : --nbFramesRestantes;
	#pragma endregion

#ifdef _DEBUG
	auto player = CMoteurWindows::GetInstance().GetCurrentScene()->GetPlayer();
	std::string pState = player.lock()->GetState();

	PM3D::CAfficheurTexte* state = new PM3D::CAfficheurTexte(200, 20, pPolice);
	std::string statestr = "STATE = " + pState;//std::to_string(/*VARIABLE HERE*/1);
	std::wstring statewstr = std::wstring(statestr.begin(), statestr.end());
	const wchar_t* statewcstr = statewstr.c_str();
	state->Ecrire(statewcstr);
	afficheur->AjouterSpriteTexte(state->GetTextureView(), 100, 500);
#endif

	auto cam = CMoteurWindows::GetInstance().GetCurrentScene()->GetCamera();
	auto posSign = DirectX::XMVectorSet(908.58f, -160.27f, 15.f, 1.f);
	if (!cam.expired()) {
		auto posCam = cam.lock().get()->GetTransform().GetPositionVector();
		auto dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(posCam - posSign));
		if (dist < 500) afficheur->AjouterPanneau("resources/sign.dds", ToXMFloat3(posSign), 3, 7);
	}
}

void UIPause::Update()
{
	if (afficheur) delete(afficheur);
	afficheur = new PM3D::CAfficheurSprite();


	afficheur->AjouterSprite("resources/PlayButton.dds", "BoutonPlay", 375, 250, 255, 160);
	afficheur->AjouterSprite("resources/OptionsButton.dds", "BoutonOptions", 230, 450, 255, 160);
	afficheur->AjouterSprite("resources/CreditsButton.dds", "BoutonCredits", 520, 445, 255, 160);
	afficheur->AjouterSprite("resources/QuitButton.dds", "BoutonQuitter", 375, 650, 255, 160);


	

	std::string choiceMenu = GetAfficheur()->getMenuChoice(PM3D::CDIManipulateur::GetInstance().GetMousePosition(PM3D::CMoteurWindows::GetInstance().getHMainWnd()));

	if (choiceMenu == "BoutonPlay")
	{
		GameManager::GetInstance().SetPause(false);
	}
	else if (choiceMenu == "BoutonOptions")
	{
		PM3D::CMoteurWindows::GetInstance().GetCurrentScene()->setMenu("OptionsMenu");
	}
	else if (choiceMenu == "BoutonCredits")
	{
		PM3D::CMoteurWindows::GetInstance().GetCurrentScene()->setMenu("CreditsMenu");
	}
	else if (choiceMenu == "BoutonQuitter")
	{
		PM3D::CMoteurWindows::GetInstance().Quit();
	}

}

void UILoading::Update()
{
	if (afficheur) delete(afficheur);
	afficheur = new PM3D::CAfficheurSprite();

	const PM3D::CDispositif* pDispositif = CMoteurWindows::GetInstance().GetDispositif();

	afficheur->AjouterSprite("resources/Loading_Screen.dds", "LoadingScreen", 
		0, pDispositif->GetHauteur(),
		pDispositif->GetLargeur(), pDispositif->GetHauteur());
}

void UISnow::Update()
{
	if (afficheur) delete(afficheur);
	afficheur = new PM3D::CAfficheurSprite();

	const PM3D::CDispositif* pDispositif = CMoteurWindows::GetInstance().GetDispositif();

	static int j = 1;
	int i;
	j % 2 == 0 ? i = j / 2 : i = (j + 1) / 2;

	std::string path = "resources/snowfall/snow (" + std::to_string(i) + ").dds";
	//std::string path = "resources/snowfall/snow (1).dds";

	afficheur->AjouterSprite(path, "SnowfallSprite",
		0, pDispositif->GetHauteur(),
		pDispositif->GetLargeur(), pDispositif->GetHauteur());
	j == 238 ? j = 1 : ++j;
}

void UIEndScreen::Update()
{
	if (afficheur) delete(afficheur);
	afficheur = new PM3D::CAfficheurSprite();

	afficheur->AjouterSprite("resources/BoutonEndScreen.dds", "BoutonEndScreen", 200, 575, 600, 400);
}

void UIResolution::Update()
{
	if (afficheur) delete(afficheur);
	afficheur = new PM3D::CAfficheurSprite();

	afficheur->AjouterSprite("resources/BoutonChangementResolution.dds", "BoutonChangementResolution", 212, 150, 600, 100);


	afficheur->AjouterSprite("resources/BoutonFenetrePleinEcran.dds", "BoutonFenetrePleinEcran", 412, 250, 200, 50);
	afficheur->AjouterSprite("resources/800x600.dds", "800x600", 412, 350, 200, 50);
	afficheur->AjouterSprite("resources/1024x768.dds", "1024x768", 412, 450, 200, 50);
	afficheur->AjouterSprite("resources/1440x1080.dds", "1440x1080", 412, 550, 200, 50);
	afficheur->AjouterSprite("resources/1600x1200.dds", "1600x1200", 412, 650, 200, 50);

	afficheur->AjouterSprite("resources/BoutonRetour.dds", "BoutonRetour", 800, 730, 75, 50);



	std::string choiceMenu = GetAfficheur()->getMenuChoice(PM3D::CDIManipulateur::GetInstance().GetMousePosition(PM3D::CMoteurWindows::GetInstance().getHMainWnd()));

	if (choiceMenu == "BoutonFenetrePleinEcran")
	{
		// DO NOTHING
	}
	else if (choiceMenu == "800x600")
	{
		//PM3D::CMoteurWindows::GetInstance().GetDispositif()->update(800, 600);
	}
	else if (choiceMenu == "1024x768")
	{
		//PM3D::CMoteurWindows::GetInstance().GetDispositif()->update(1024, 768);
	}
	else if (choiceMenu == "1440x1080")
	{
		//PM3D::CMoteurWindows::GetInstance().GetDispositif()->update(1440, 1080);
	}
	else if (choiceMenu == "1600x1200")
	{
		//PM3D::CMoteurWindows::GetInstance().GetDispositif()->update(1600, 1200);
	}
	else if (choiceMenu == "BoutonRetour")
	{
		PM3D::CMoteurWindows::GetInstance().GetCurrentScene()->setMenu("PauseMenu");
	}


}

void UICredits::Update()
{
	/*std::string choiceMenu = GetAfficheur()->getMenuChoice(PM3D::CDIManipulateur::GetInstance().GetMousePosition(PM3D::CMoteurWindows::GetInstance().getHMainWnd()));

	if (choiceMenu == "Retour")
	{
		PM3D::CMoteurWindows::GetInstance().GetCurrentScene()->setMenu("PauseMenu");
	}*/
}
