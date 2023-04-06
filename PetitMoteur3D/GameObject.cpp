#include "stdafx.h"
#include "GameObject.h"
#include "Component.h"
#include "Scene.h"
#include "Camera.h"
#include "MoteurWindows.h"

using namespace std;
using namespace PM3D;
using namespace physx;
using namespace DirectX;

GameObject::~GameObject()
{
	isLoaded = false;

	if (actor) {
		PX_RELEASE(actor);
	}

	components.clear();
}

std::string GameObject::GetName() const noexcept
{
	return name;
}

int GameObject::GetId() const noexcept
{
	return id;
}

bool GameObject::HasMesh() const noexcept
{
	return mesh != nullptr;
}

PM3D::CObjet3D& GameObject::GetMesh() const
{
	return *mesh;
}

PM3D::CObjet3D& GameObject::GetMesh()
{
	return *mesh;
}

bool GameObject::HasActor() const noexcept
{
	return actor != nullptr;
}

physx::PxRigidActor& GameObject::GetActor() const
{
	return *actor;
}

physx::PxRigidActor& GameObject::GetActor()
{
	return *actor;
}

CTransform GameObject::GetTransform() const noexcept
{
	return transform;
}

CTransform& GameObject::GetTransform() noexcept
{
	return transform;
}


#pragma region Protected

void GameObject::PxTransform2CTransform()
{
	if (!HasActor()) return;

	const PxTransform pxTransform = actor->getGlobalPose();
	const PxVec3 position = pxTransform.p;
	const PxQuat quatRot = pxTransform.q;

	transform.SetPosition(position.x, position.y, position.z);
	transform.SetRotation(XMVectorSet(quatRot.x, quatRot.y, quatRot.z, quatRot.w));
}

void GameObject::CTransform2PxTransform()
{
	if (!HasActor()) return;

	const XMFLOAT3 position = transform.GetPositionFloat3();
	const XMVECTOR rotation = transform.GetRotationQuaternion();

	PxTransform pxTransform = actor->getGlobalPose();
	pxTransform.p = PxVec3(position.x, position.y, position.z);
	pxTransform.q = PxQuat(XMVectorGetX(rotation), XMVectorGetY(rotation), XMVectorGetZ(rotation), XMVectorGetW(rotation));
	actor->setGlobalPose(pxTransform);
}

#pragma endregion


#pragma region Privates

void GameObject::Load()
{
	if (isLoaded) throw new gameobject_exception("Already Loaded");

	mesh = unique_ptr<CObjet3D>(InitMesh());
	actor = InitActor();
	InitComponents();

	isLoaded = true;
	PostLoadInit();
}

void GameObject::Start()
{
	if (!isLoaded) throw new gameobject_exception("Not loaded");

	OnStart();

	// Start all components
	for (shared_ptr<Component>& comp : components)
	{
		comp->Start();
	}

	//Synchronize transforms
	CTransform2PxTransform();
}

void GameObject::Update()
{
	if (!isLoaded) throw new gameobject_exception("Not loaded");

	OnUpdate();

	// Update all components
	for (shared_ptr<Component>& comp : components)
	{
		comp->Update();
	}

	//Synchronize transforms
	CTransform2PxTransform();
}

void GameObject::PhysicUpdate()
{
	if (!HasActor()) return;

	//Synchronize transforms
	PxTransform2CTransform();
}

void GameObject::CollisionEnter(GameObject& other)
{
	OnCollisionEnter(other);
	for (shared_ptr<Component>& comp : components)
	{
		comp->OnCollisionEnter(other);
	}
}

void GameObject::CollisionStay(GameObject& other)
{
	OnCollisionStay(other);
	for (shared_ptr<Component>& comp : components)
	{
		comp->OnCollisionStay(other);
	}
}

void GameObject::CollisionExit(GameObject& other)
{
	OnCollisionExit(other);
	for (shared_ptr<Component>& comp : components)
	{
		comp->OnCollisionExit(other);
	}
}

void GameObject::TriggerEnter(GameObject& other)
{
	OnTriggerEnter(other);
	for (shared_ptr<Component>& comp : components)
	{
		comp->OnTriggerEnter(other);
	}
}

void GameObject::TriggerStay(GameObject& other)
{
	OnTriggerStay(other);
	for (shared_ptr<Component>& comp : components)
	{
		comp->OnTriggerStay(other);
	}
}

void GameObject::TriggerExit(GameObject& other)
{
	OnTriggerExit(other);
	for (shared_ptr<Component>& comp : components)
	{
		comp->OnTriggerExit(other);
	}
}

void GameObject::Draw(const CCamera& camera, const CLight& light) const
{
	if (!isLoaded) throw new gameobject_exception("Not loaded");
	if (!HasMesh()) return;

	mesh->Draw(camera, light, transform);
}

#pragma endregion