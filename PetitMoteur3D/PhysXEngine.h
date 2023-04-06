#pragma once
#include "Singleton.h"
#include "PxPhysicsAPI.h"
#include "foundation/PxPreprocessor.h"
#include "Layer.h"
#include <memory>

#define PX_RELEASE(x) if(x) { x->release(); x=NULL; }

template <class T> inline void PhysXRelease(T* t)
{
	if (t) t->release();
}

struct SceneData
{
	float xGrav = 0.0f;
	float yGrav = -9.81f;
	float zGrav = 0.0f;
	physx::PxSimulationEventCallback* cbSimulationEvent;
	physx::PxContactModifyCallback* cbContactModify;
};

struct MaterialData
{
	float staticFriction = 0.05f;
	float dynamicFriction = 0.05f;
	float restitution = 1.0f;
};

struct RigidActorData
{
	int id;
	Layer layer = Layer::LayerA;
	bool isTrigger = false;
	bool useGravity = true;

	RigidActorData(int id) noexcept : id{id} { /*vide*/ }
};

struct RigidDynamicData : public RigidActorData
{
	float mass = 1.0f;
	float linearDamping = 0.1f;
	float angularDamping = 0.1f;
	physx::PxRigidDynamicLockFlags lockFlags;
};

class PhysXEngine : CSingleton<PhysXEngine>
{
	friend class CSingleton<PhysXEngine>;

	physx::PxDefaultAllocator		gAllocator;
	physx::PxDefaultErrorCallback	gErrorCallback;

	physx::PxFoundation* gFoundation = nullptr;
	physx::PxPhysics* gPhysics = nullptr;
	physx::PxDefaultCpuDispatcher* gDispatcher = nullptr;
	physx::PxPvd* gPvd = nullptr;

	physx::PxCooking* gCooking = nullptr;

	void SetupFiltering(physx::PxRigidActor* actor, const RigidActorData& data) const;

protected:
	PhysXEngine();
	~PhysXEngine();

public:
	using unique_scene_t = std::unique_ptr<physx::PxScene, void(physx::PxScene*)>;

	physx::PxPhysics& GetPhysics() const noexcept;

	static physx::PxScene* MakeScene(const SceneData& data);
	static physx::PxMaterial* CreateMaterial(const MaterialData& data);
	static physx::PxRigidDynamic* CreateRigidDynamic(const RigidDynamicData& data, const physx::PxGeometry& geometry, const physx::PxMaterial& material);
	static physx::PxRigidDynamic* CreateRigidDynamic(const RigidDynamicData& data, const physx::PxGeometry& geometry, const MaterialData& matData);
	static physx::PxRigidStatic* CreateRigidStatic(const RigidActorData& data, const physx::PxGeometry& geometry, const physx::PxMaterial& material);
	static physx::PxRigidStatic* CreateRigidStatic(const RigidActorData& data, const physx::PxGeometry& geometry, const MaterialData& matData);
	static physx::PxTriangleMeshGeometry CreateTriangleMesh(physx::PxTriangleMeshDesc& desc, physx::PxVec3 scale = physx::PxVec3{1, 1, 1});
};