#pragma once
#include <vector>
#include <memory>
#include <exception>
#include "PhysXEngine.h"
#include "Camera.h"
#include "GameObject.h"
#include "Player.h"
#include "TerrainObject.h"
#include "Skybox.h"
#include "Light.hpp"
#include "AfficheurSprite.h"
#include "CTerrain.h"
#include "SoundManager.h"


class scene_exception : public std::exception {
	const char* msg;

public:
	scene_exception(const char* msg) : msg{msg} { /*vide*/ }
	scene_exception(const scene_exception& e) : msg{e.msg} { /*msg*/ }
	const char* what() const override { return msg; }
};

class Scene
{
private:
	struct PhysXCallbacks : physx::PxSimulationEventCallback, physx::PxContactModifyCallback
	{
		PhysXCallbacks(const Scene& scene);
		~PhysXCallbacks() = default;

		void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
		void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
		void onContactModify(physx::PxContactModifyPair* const pairs, physx::PxU32 count) override { }

		void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override {}
		void onWake(physx::PxActor**, physx::PxU32) override {}
		void onSleep(physx::PxActor**, physx::PxU32) override {}
		void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) override {}

	private:
		const Scene& scene;
	} callbacks{*this};

	bool isLoaded = false;
	
	std::weak_ptr<CCamera> camera;
	std::weak_ptr<CLight> light;

	std::shared_ptr<Player> player;
	std::vector<std::shared_ptr<GameObject>> objects;

	std::vector<std::shared_ptr<GameObject>> toAdd;
	std::vector<GameObject*> toDelete;

	physx::PxScene* physxScene;

public:
	Scene() = default;
	virtual ~Scene();

	void Load();
	void Start();
	void Update();
	void Render() const;
	void Close();

	//---------- Setter ----------//
	void SetCamera(std::shared_ptr<CCamera>);
	void SetLight(std::shared_ptr<CLight>);
	std::weak_ptr<GameObject> AddObjectToScene(GameObject* object);
	void RemoveObjectFromScene(GameObject* object);
	void setMenu(std::string nomMenu);

	//---------- Getter ----------//
	std::weak_ptr<Player> GetPlayer() const noexcept;
	std::weak_ptr<CCamera> GetCamera() const noexcept;
	std::weak_ptr<CLight> GetLight() const noexcept;
	std::weak_ptr<GameObject> GetGameObjectByName(std::string name) const noexcept;
	std::weak_ptr<GameObject> GetGameObjectById(int id) const noexcept;

protected:
	virtual std::shared_ptr<Player> InitPlayer() = 0;
	//virtual std::shared_ptr<CCamera> InitCamera() = 0;
	//virtual std::shared_ptr<CLight> InitLight() = 0;
	virtual void InitObjects() { /*À implémenter si nécessaire*/ }
	virtual void InitOthers() { /*À implémenter si nécessaire*/ }

	virtual void PostInitLoad() { /*À implémenter si nécessaire*/ }
	virtual void OnStart() { /*À implémenter si nécessaire*/ }
	virtual void OnUpdate() { /*À implémenter si nécessaire*/ }
	virtual void OnRender() const { /*À implémenter si nécessaire*/ }
	virtual void OnClose() { /*À implémenter si nécessaire*/ }

	virtual void UIRender() const { /*À implémenter si nécessaire*/ }

	std::string currentMenu = "";
private:
	void initPhysic();
	void playScene();

	void removeObjects();
	void addObjects();
};
