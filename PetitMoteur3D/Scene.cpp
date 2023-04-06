#include "stdafx.h"
#include "Scene.h"
#include "Time.h"
#include <string>
#include "AfficheurTexte.h"
#include <iomanip>
#include "MoteurWindows.h"
#include <Windows.h>
#include <mmsystem.h>

using namespace std;
using namespace physx;
using namespace DirectX;
#pragma comment(lib, "winmm.lib")

#pragma region PhysXCallbacks

Scene::PhysXCallbacks::PhysXCallbacks(const Scene& scene) : scene{scene}
{
	//Vide
}

void Scene::PhysXCallbacks::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; ++i)
	{
		const PxContactPair& cp = pairs[i];

		//Find the objects
		const PxU32 id0 = cp.shapes[0]->getSimulationFilterData().word1;
		const PxU32 id1 = cp.shapes[1]->getSimulationFilterData().word1;

		weak_ptr<GameObject> obj0 = scene.GetGameObjectById(id0);
		weak_ptr<GameObject> obj1 = scene.GetGameObjectById(id1);

		if (!obj0.expired() && !obj1.expired())
		{
			//Call the right type of collisions
			if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				obj0.lock()->CollisionEnter(*(obj1.lock()));
				obj1.lock()->CollisionEnter(*(obj0.lock()));
			}
			else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				obj0.lock()->CollisionExit(*(obj1.lock()));
				obj1.lock()->CollisionExit(*(obj0.lock()));
			}
			else if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
			{
				obj0.lock()->CollisionStay(*(obj1.lock()));
				obj1.lock()->CollisionStay(*(obj0.lock()));
			}
		}
	}
}

void Scene::PhysXCallbacks::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (PxU32 i = 0; i < count; ++i)
	{
		const PxTriggerPair& tp = pairs[i];

		//Find the objects
		const PxU32 triggerId = tp.triggerShape->getSimulationFilterData().word1;
		const PxU32 otherId = tp.otherShape->getSimulationFilterData().word1;

		weak_ptr<GameObject> triggerObj = scene.GetGameObjectById(triggerId);
		weak_ptr<GameObject> otherObj = scene.GetGameObjectById(otherId);

		if (!triggerObj.expired() && !otherObj.expired())
		{
			//Call the right type of collisions
			if (tp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				triggerObj.lock()->TriggerEnter(*(otherObj.lock()));
			}
			else if (tp.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				triggerObj.lock()->TriggerExit(*(otherObj.lock()));
			}
			else if (tp.status & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
			{
				triggerObj.lock()->TriggerStay(*(otherObj.lock()));
			}
		}
	}
}

#pragma endregion

Scene::~Scene()
{
	if (isLoaded)
	{
		Close();
	}
}

void Scene::Load()
{
	if (isLoaded) throw make_unique<exception>(scene_exception("Cannot load a scene twice"));

	
	
	// Initialiser la physique
	initPhysic();

	// Le joueur
	player = InitPlayer();
	if (player) {
		player->Load();
		physxScene->addActor(player->GetActor());
	}

	// Les objects
	InitObjects();
	for (shared_ptr<GameObject>& obj : toAdd)
	{
		obj->Load();
	}

	//Autre
	InitOthers();

	isLoaded = true;
	PostInitLoad();

	GameManager::GetInstance().SetPause(true);
}

void Scene::Start()
{
	if (!isLoaded) throw make_unique<exception>(scene_exception("Scene not loaded"));

	OnStart();

	if (player) player->Start();

	

	// Add gameobjects to the scene (starting them)
	addObjects();

	if(!camera.expired()) camera.lock()->Start();
}

void Scene::Update() 
{
	if (!isLoaded) throw make_unique<exception>(scene_exception("Scene not loaded"));

	toDelete.clear();
	toAdd.clear();

	OnUpdate();

	if (GameManager::GetInstance().IsInLevel() 
		&& !GameManager::GetInstance().IsPaused())
	{
		playScene();
	}

	// Update the camera
	if (!camera.expired()) camera.lock()->Update();

	//----- After update -----//
	removeObjects();
	addObjects();
}

void Scene::Render() const
{
	if (!isLoaded) throw make_unique<exception>(scene_exception("Scene not loaded"));

	OnRender();

	if (camera.expired() || light.expired()) return;
	std::shared_ptr<CCamera> pCamera = camera.lock();
	std::shared_ptr<CLight> pLight = light.lock();

	if (player) player->Draw(*pCamera, *pLight);

	// Draw all objects
	for (auto& obj : objects)
	{
		obj->Draw(*pCamera, *pLight);
	}

	UIRender();
}

void Scene::Close()
{
	if (!isLoaded) throw make_unique<exception>(scene_exception("Scene not loaded"));
	isLoaded = false;

	OnClose();

	// Fermer la sc�ne physx
	if(player) physxScene->removeActor(player->GetActor());
	for (auto& obj : objects)
	{
		if(obj->HasActor()) physxScene->removeActor(obj->GetActor());
	}
	PX_RELEASE(physxScene);

	// Supprimer les objets
	objects.clear();
	toAdd.clear();
	toDelete.clear();

	player = nullptr;
}

#pragma region Setter

void Scene::SetCamera(std::shared_ptr<CCamera> _camera)
{
	camera = _camera;
	_camera->Start();
}

void Scene::SetLight(std::shared_ptr<CLight> _light)
{
	light = _light;
}

weak_ptr<GameObject> Scene::AddObjectToScene(GameObject* object)
{
	shared_ptr<GameObject> pObj = shared_ptr<GameObject>(object);
	toAdd.push_back(pObj);

	return weak_ptr<GameObject>(pObj);
}

void Scene::RemoveObjectFromScene(GameObject* object)
{
	toDelete.push_back(object);
}

void Scene::setMenu(std::string nomMenu)
{
	currentMenu = nomMenu;
}
#pragma endregion


#pragma region Getter

std::weak_ptr<Player> Scene::GetPlayer() const noexcept
{
	return weak_ptr<Player>(player);
}

std::weak_ptr<CCamera> Scene::GetCamera() const noexcept
{
	return camera;
}

std::weak_ptr<CLight> Scene::GetLight() const noexcept
{
	return light;
}

std::weak_ptr<GameObject> Scene::GetGameObjectByName(std::string name) const noexcept
{
	auto pred = [&name](const shared_ptr<GameObject>& obj) {
		return obj->GetName() == name;
	};
	auto it = find_if(begin(objects), end(objects), pred);

	if (it == end(objects))
	{
		if (player && player->GetName() == name)
		{
			return player;
		}

		return weak_ptr<GameObject>();		//return empty pointer
	}

	return *it;
}

std::weak_ptr<GameObject> Scene::GetGameObjectById(int id) const noexcept
{
	auto pred = [&id](const shared_ptr<GameObject>& obj) {
		return obj->GetId() == id;
	};
	auto it = find_if(begin(objects), end(objects), pred);

	if (it == end(objects))
	{
		if (player && player->GetId() == id)
		{
			return player;
		}

		return weak_ptr<GameObject>();		//return empty pointer
	}

	return *it;
}

#pragma endregion


#pragma region Privates

void Scene::initPhysic()
{
	SceneData data{};
	data.yGrav = -9.81f;
	data.cbSimulationEvent = &callbacks;
	data.cbContactModify = &callbacks;

	physxScene = PhysXEngine::MakeScene(data);
}

void Scene::playScene()
{
	//----- Update -----//
	if (player) player->Update();

	// Update all gameobjects
	for (auto& obj : objects)
	{
		obj->Update();
	}

	//----- Simulation -----//
	physxScene->simulate(Time::GetFixedDeltaTime());
	physxScene->fetchResults(true);

	//----- Physics Update-----/
	if(player) player->PhysicUpdate();

	for (auto& obj : objects)
	{
		obj->PhysicUpdate();
	}
}

void Scene::removeObjects()
{
	auto pred = [this](const shared_ptr<GameObject>& obj) {
		if (find(begin(toDelete), end(toDelete), obj.get()) != end(toDelete))
		{
			if(obj->HasActor()) physxScene->removeActor(obj->GetActor());
			return true;
		}

		return false;
	};

	auto it = remove_if(begin(objects), end(objects), pred);
	objects.erase(it, end(objects));

	toDelete.clear();
}

void Scene::addObjects()
{
	for (shared_ptr<GameObject> obj : toAdd)
	{
		objects.push_back(obj);
		if(obj->HasActor()) physxScene->addActor(obj->GetActor());

		obj->Start();
	}

	toAdd.clear();
}

#pragma endregion