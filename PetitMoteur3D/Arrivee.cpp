#include "stdafx.h"
#include "Arrivee.h"
#include "PhysXEngine.h"
#include "MoteurWindows.h"

using namespace physx;
using namespace std;

Arrivee::Arrivee(std::string nom) noexcept
	: GameObject(nom)
{
	//vide
}

Arrivee::Arrivee(std::string nom,
	float pos_x, float pos_y, float pos_z,
	float rot_x, float rot_y, float rot_z,
	float scale_x, float scale_y, float scale_z) noexcept : Arrivee(nom)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
	this->GetTransform().SetScale(scale_x, scale_y, scale_z);
}

Arrivee::Arrivee(std::string nom,
	float pos_x, float pos_y, float pos_z,
	float rot_x, float rot_y, float rot_z,
	float scale_xyz) noexcept : Arrivee(nom)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
	this->GetTransform().SetScale(scale_xyz);
}

physx::PxRigidActor* Arrivee::InitActor()
{
	PxBoxGeometry geo(ToPxVec3(GetTransform().GetScaleFloat3()));

	RigidActorData data{ id };
	data.useGravity = false;
	data.isTrigger = true;
	data.layer = Layer::Trigger;

	MaterialData matData{};

	return PhysXEngine::CreateRigidStatic(data, geo, matData);
}

void Arrivee::OnTriggerEnter(GameObject& other)
{
	//check if player

	Scene* currentScene = PM3D::CMoteurWindows::GetInstance().GetCurrentScene();
	MainScene* mainScene = dynamic_cast<MainScene*>(currentScene);

	if (mainScene)
	{
		mainScene->Reset();
	}
}

void Arrivee::OnTriggerStay(GameObject& other)
{
}

void Arrivee::OnTriggerExit(GameObject& other)
{
	//check if player

	
}
