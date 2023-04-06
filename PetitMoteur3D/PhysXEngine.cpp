#include "stdafx.h"
#include "PhysXEngine.h"

#define PVD_HOST "127.0.0.1"

using namespace physx;
using namespace std;

PxFilterFlags CustomFilterShader(
	PxFilterObjectAttributes attr0, PxFilterData data0, 
	PxFilterObjectAttributes attr1, PxFilterData data1, 
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	if (LayerCollisions[data0.word0][data1.word0])
	{
		if (PxFilterObjectIsTrigger(attr0) || PxFilterObjectIsTrigger(attr1))
		{
			pairFlags |= PxPairFlag::eTRIGGER_DEFAULT;
		}
		else
		{
			pairFlags |= PxPairFlag::eCONTACT_DEFAULT;
		}

		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
		pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;

		return PxFilterFlag::eDEFAULT;
	}

	return PxFilterFlag::eKILL;
}

void PhysXEngine::SetupFiltering(physx::PxRigidActor* actor, const RigidActorData& data) const
{
	PxFilterData filterData;
	filterData.word0 = static_cast<PxU32>(data.layer);
	filterData.word1 = static_cast<PxU32>(data.id);

	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = new PxShape * [numShapes];
	actor->getShapes(shapes, numShapes);

	for (PxU32 i = 0; i < numShapes; ++i)
	{
		shapes[i]->setSimulationFilterData(filterData);
		shapes[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !data.isTrigger);
		shapes[i]->setFlag(PxShapeFlag::eTRIGGER_SHAPE, data.isTrigger);
	}

	delete[] shapes;
}

physx::PxPhysics& PhysXEngine::GetPhysics() const noexcept
{
	return *gPhysics;
}

PhysXEngine::PhysXEngine()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);

	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(gPhysics->getTolerancesScale()));
}

PhysXEngine::~PhysXEngine()
{
	PX_RELEASE(gCooking);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
}

PxScene* PhysXEngine::MakeScene(const SceneData& data)
{
	PhysXEngine& engine = PhysXEngine::GetInstance();

	PxSceneDesc sceneDesc(engine.gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(data.xGrav, data.yGrav, data.zGrav);
	sceneDesc.cpuDispatcher	= engine.gDispatcher;
	sceneDesc.filterShader = CustomFilterShader;
	sceneDesc.simulationEventCallback = data.cbSimulationEvent;
	sceneDesc.contactModifyCallback = data.cbContactModify;
	sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
	
	PxScene* scene = engine.gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
	if(pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	return scene;
}

physx::PxMaterial* PhysXEngine::CreateMaterial(const MaterialData& data)
{
	return PhysXEngine::GetInstance().gPhysics->createMaterial(PxReal{ data.staticFriction }, PxReal{ data.dynamicFriction }, PxReal{ data.restitution });
}

physx::PxRigidDynamic* PhysXEngine::CreateRigidDynamic(const RigidDynamicData& data, const physx::PxGeometry& geometry, const physx::PxMaterial& material)
{
	PhysXEngine& engine = PhysXEngine::GetInstance();

	const PxTransform transform{ 0, 0, 0 };

	PxRigidDynamic* actor = engine.gPhysics->createRigidDynamic(transform);
	PxRigidActorExt::createExclusiveShape(*actor, geometry, material);

	engine.SetupFiltering(actor, data);

	actor->setMass(PxReal{ data.mass });
	actor->setLinearDamping(PxReal{ data.linearDamping });
	actor->setAngularDamping(PxReal{ data.angularDamping });

	actor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
	actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !data.useGravity);
	actor->setRigidDynamicLockFlags(data.lockFlags);

	return actor;
}

physx::PxRigidDynamic* PhysXEngine::CreateRigidDynamic(const RigidDynamicData& data, const physx::PxGeometry& geometry, const MaterialData& matData)
{
	PxMaterial* material = CreateMaterial(matData);
	PxRigidDynamic* actor = CreateRigidDynamic(data, geometry, *material);

	PX_RELEASE(material);
	return actor;
}

physx::PxRigidStatic* PhysXEngine::CreateRigidStatic(const RigidActorData& data, const physx::PxGeometry& geometry, const physx::PxMaterial& material)
{
	PhysXEngine& engine = PhysXEngine::GetInstance();

	const PxTransform transform{ 0, 0, 0 };

	PxRigidStatic* actor = engine.gPhysics->createRigidStatic(transform);
	PxRigidActorExt::createExclusiveShape(*actor, geometry, material);

	engine.SetupFiltering(actor, data);

	actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !data.useGravity);

	return actor;
}

physx::PxRigidStatic* PhysXEngine::CreateRigidStatic(const RigidActorData& data, const physx::PxGeometry& geometry, const MaterialData& matData)
{
	PxMaterial* material = CreateMaterial(matData);

	PxRigidStatic* actor = CreateRigidStatic(data, geometry, *material);
	PX_RELEASE(material);

	return actor;
}

physx::PxTriangleMeshGeometry PhysXEngine::CreateTriangleMesh(physx::PxTriangleMeshDesc& desc, physx::PxVec3 scale)
{
	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	
	PhysXEngine& engine = PhysXEngine::GetInstance();
	bool status = engine.gCooking->cookTriangleMesh(desc, writeBuffer, &result);
	if (!status)
	{
		throw result;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* triMesh = engine.gPhysics->createTriangleMesh(readBuffer);

	PxMeshScale meshScale{ scale };
	const PxTriangleMeshGeometry geometry{ triMesh, scale };

	return geometry;
}
