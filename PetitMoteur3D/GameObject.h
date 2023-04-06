#pragma once
#include <vector>
#include <memory>
#include <string>
#include <exception>
#include "Objet3D.h"
#include "Transform.h"
#include "PhysXEngine.h"
#include "DIManipulateur.h"
#include "Time.h"

struct gameobject_exception : std::exception {
	const char* msg;

	gameobject_exception(const char* msg) : msg{ msg } { /*vide*/ }
	const char* what() const override { return msg; }
};

class Component;
class Scene;
class CCamera;

class GameObject
{
	static inline int CURRENT_ID = 0;

	friend class Scene;

	bool isLoaded = false;

protected:
	const int id;
	const std::string name;

	CTransform transform{};

	/*std::unique_ptr<PM3D::CGroupMeshes> meshes;*/
	std::unique_ptr<PM3D::CObjet3D> mesh;
	physx::PxRigidActor* actor;

	std::vector<std::shared_ptr<Component>> components;

public:
	GameObject(std::string name) noexcept : name{ name }, id{ CURRENT_ID++ } { /*vide*/ }
	GameObject(const GameObject& other) = delete;
	virtual ~GameObject();

	GameObject& operator=(const GameObject& other) = delete;

	//---------- Getter ----------//
	std::string GetName() const noexcept;
	int GetId() const noexcept;

	bool HasMesh() const noexcept;
	PM3D::CObjet3D& GetMesh() const;
	PM3D::CObjet3D& GetMesh();

	bool HasActor() const noexcept;
	physx::PxRigidActor& GetActor() const;
	physx::PxRigidActor& GetActor();

	CTransform GetTransform() const noexcept;
	CTransform& GetTransform() noexcept;

	template <class T> T* GetComponent()
	{
		const std::type_info& tInfo = typeid(T);

		for (std::shared_ptr<Component>& c : components)
		{
			const std::type_info& cInfo = typeid(*c);
			if (tInfo == cInfo)
			{
				return static_cast<T*>(c.get());
			}
		}

		return nullptr;
	}

protected:
	void PxTransform2CTransform();
	void CTransform2PxTransform();

	virtual PM3D::CObjet3D* InitMesh() = 0;
	virtual physx::PxRigidActor* InitActor() = 0;
	virtual void InitComponents() { /*� impl�menter si n�cessaire*/ }
	virtual void PostLoadInit() { /*� impl�menter si n�cessaire*/ }

	virtual void OnStart() { /*� impl�menter si n�cessaire*/ }
	virtual void OnUpdate() { /*� impl�menter si n�cessaire*/ }
	virtual void OnCollisionEnter(GameObject& other) { /*À IMPLÉMENTER*/ }
	virtual void OnCollisionStay(GameObject& other) { /*À IMPLÉMENTER*/ }
	virtual void OnCollisionExit(GameObject& other) { /*À IMPLÉMENTER*/ }
	virtual void OnTriggerEnter(GameObject& other) { /*À IMPLÉMENTER*/ }
	virtual void OnTriggerStay(GameObject& other) { /*À IMPLÉMENTER*/ }
	virtual void OnTriggerExit(GameObject& other) { /*À IMPLÉMENTER*/ }

	virtual void Draw(const CCamera& camera, const CLight& light) const;

	void Load();
	void Start();
	void Update();

	void PhysicUpdate();
	void CollisionEnter(GameObject& other);
	void CollisionStay(GameObject& other);
	void CollisionExit(GameObject& other);
	void TriggerEnter(GameObject& other);
	void TriggerStay(GameObject& other);
	void TriggerExit(GameObject& other);
};