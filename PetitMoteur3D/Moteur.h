#pragma once
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include "Singleton.h"
#include "dispositif.h"
#include "DIManipulateur.h"
#include "GestionnaireDeTexture.h"
#include "Time.h"
#include "GameManager.h"
#include "SoundManager.h"
#include "SceneDef.h"

namespace PM3D
{

	constexpr const int IMAGESPARSECONDE = 60 ;
	const double EcartTemps = 1.0 / static_cast<double>(IMAGESPARSECONDE);

	//
	//   TEMPLATE�: CMoteur
	//
	//   BUT�: Template servant � construire un objet Moteur qui implantera les
	//         aspects "g�n�riques" du moteur de jeu
	//
	//   COMMENTAIRES�:
	//
	//        Comme plusieurs de nos objets repr�senteront des �l�ments uniques 
	//        du syst�me (ex: le moteur lui-m�me, le lien vers 
	//        le dispositif Direct3D), l'utilisation d'un singleton 
	//        nous simplifiera plusieurs aspects.
	//
	template <class T, class TClasseDispositif> class CMoteur : public CSingleton<T>
	{
		class SceneLoader
		{
			Scene* sceneToLoad;
			std::atomic<bool> doneLoading;

			std::thread* th = nullptr;

		public:
			SceneLoader(Scene* scene) : sceneToLoad{scene}, doneLoading{ false }
			{ 
				auto load = [](std::atomic<bool>& finished, Scene* scene) {
					scene->Load();
					finished = true;
				};

				th = new std::thread(load, std::ref(doneLoading), sceneToLoad);
			}
			~SceneLoader() 
			{
				if (th)
				{
					if(th->joinable()) th->join();
					delete th;
				}
			}

			bool isDone() const { return doneLoading; }
			Scene* getScene() const { return sceneToLoad; }
		};

		bool bBoucle = true;

	public:
		virtual void Run()
		{
			while (bBoucle)
			{
				// Pour le chargement de scène
				if (loader && loader->isDone())
				{
					currentScene = loader->getScene();		//Passer à la scène chargée
					loader = nullptr;						//Arrêtez la thread de chargement
					currentScene->Start();					//Startez la scène
				}

				// Propre � la plateforme - (Conditions d'arr�t, interface, messages)
				bBoucle = RunSpecific();

				// appeler la fonction d'animation
				if (bBoucle)
				{
					Animation();
				}
			}
		}

		virtual int Initialisations()
		{
			// Propre � la plateforme
			InitialisationsSpecific();

			// * Initialisation du dispositif de rendu
			pDispositif = CreationDispositifSpecific(CDS_FENETRE);

			// * Scenes
			RegisterScene();

			// * Initialisation du jeu
			InitGame();

			// * Initialisation des param�tres de l'animation et 
			//   pr�paration de la premi�re image
			InitAnimation();

			return 0;
		}

		virtual void Close()
		{
			if (currentScene) {
				currentScene->Close();
				currentScene = nullptr;
			}
			loadingScreen->Close();

			Time::GetInstance().Close();
			CDIManipulateur::GetInstance().Close();
			GameManager::GetInstance().Close();
			PM3D::CAfficheurTexte::Close();
			SoundManager::GetInstance().Close();

			CloseSpecific();
		}

		virtual void Animation()
		{
			// m�thode pour lire l'heure et calculer le 
			// temps �coul�
			double TempsEcoule = Time::GetTimeSinceLastUpdate();

			// Est-il temps de rendre l'image?
			if (TempsEcoule > EcartTemps)
			{
				// Affichage optimis�
				pDispositif->Present();

				// Update des gestionnaire
				UpdateGame();
				
				// On pr�pare la prochaine image
				AnimeScene();

				// On rend l'image sur la surface de travail
				// (tampon d'arri�re plan)
				RenderScene();
			}
		}

		virtual void Quit()
		{
			bBoucle = false;
		}

		TClasseDispositif* GetDispositif() const noexcept { return pDispositif; }
		CGestionnaireDeTextures& GetTextureManager() { return TexturesManager; }

		Scene* GetCurrentScene() const { return currentScene; }

	protected:
		virtual ~CMoteur()
		{
			Cleanup();
		}

		//---------- Specifique ----------//
		virtual bool RunSpecific() = 0;
		virtual int InitialisationsSpecific() = 0;
		virtual void CloseSpecific() = 0;
		virtual TClasseDispositif* CreationDispositifSpecific(const CDS_MODE cdsMode) = 0;
		virtual void BeginRenderSceneSpecific() = 0;
		virtual void EndRenderSceneSpecific() = 0;

		//---------- Initialisation ----------//
		virtual void Cleanup()
		{
			scenes.clear();

			// D�truire le dispositif
			if (pDispositif)
			{
				delete pDispositif;
				pDispositif = nullptr;
			}
		}

		virtual void RegisterScene()
		{
			loadingScreen = std::make_unique<LoadingScreen>();
			loadingScreen->Load();
			loadingScreen->Start();

			scenes.push_back(std::make_unique<MainScene>());
		}

		virtual void InitGame()
		{
			if (scenes.empty()) throw;

			PM3D::CAfficheurTexte::Init();
			Time::GetInstance().Start();
			CDIManipulateur::GetInstance().Start();
			GameManager::GetInstance().Start();
			SoundManager::GetInstance().Start();

			Scene* firstScene = scenes[0].get();
			if (!LoadScene(firstScene)) throw;

			SoundManager::GetInstance().playSong("resources/sounds/music.wav", "music", 30, 0.3);
		}

		virtual int InitAnimation()
		{
			// premi�re Image
			RenderScene();

			return true;
		}

		//---------- Execution ----------//
		bool LoadScene(Scene* scene)
		{
			if (currentScene) currentScene->Close();

			//Afficher le loading screen
			currentScene = loadingScreen.get();

			//Commence une thread pour le chargement de la scène
			loader = std::make_unique<SceneLoader>(scene);

			return true;
		}

		bool UpdateGame()
		{
			Time::GetInstance().Update();
			CDIManipulateur::GetInstance().Update();
			GameManager::GetInstance().Update();
			SoundManager::GetInstance().Update();

			return true;
		}

		bool AnimeScene()
		{
			if (currentScene) currentScene->Update();

			return true;
		}

		bool RenderScene()
		{
			BeginRenderSceneSpecific();

			if (currentScene) currentScene->Render();

			EndRenderSceneSpecific();
			return true;
		}

	protected:
		// Le dispositif de rendu
		TClasseDispositif* pDispositif = nullptr;

		// Les scenes
		std::vector<std::unique_ptr<Scene>> scenes;
		Scene* currentScene = nullptr;

		std::unique_ptr<LoadingScreen> loadingScreen = nullptr;
		std::unique_ptr<SceneLoader> loader = nullptr;

		//---------- Gestionnaire ----------//
		CGestionnaireDeTextures TexturesManager;
	};

} // namespace PM3D
