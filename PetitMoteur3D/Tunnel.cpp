#include "stdafx.h"
#include "Tunnel.h"
#include "PhysXEngine.h"
#include "MoteurWindows.h"

using namespace physx;
using namespace std;

Tunnel::Tunnel(std::string nom, CLight* light) noexcept 
	: GameObject(nom), tunnelLight{ shared_ptr<CLight>(light) }
{
	//vide
}

Tunnel::Tunnel(std::string nom, CLight* light,
	float pos_x, float pos_y, float pos_z, 
	float rot_x, float rot_y, float rot_z, 
	float scale_x, float scale_y, float scale_z) noexcept : Tunnel(nom, light)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
	this->GetTransform().SetScale(scale_x, scale_y, scale_z);
}

Tunnel::Tunnel(std::string nom, CLight* light,
	float pos_x, float pos_y, float pos_z, 
	float rot_x, float rot_y, float rot_z, 
	float scale_xyz) noexcept : Tunnel(nom, light)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
	this->GetTransform().SetScale(scale_xyz);
}

Tunnel::~Tunnel()
{
	CLight* light = tunnelLight.get();
}

physx::PxRigidActor* Tunnel::InitActor()
{
	PxBoxGeometry geo(ToPxVec3(GetTransform().GetScaleFloat3()));

	RigidActorData data{ id };
	data.useGravity = false;
	data.isTrigger = true;
	data.layer = Layer::Trigger;

	MaterialData matData{};

	return PhysXEngine::CreateRigidStatic(data, geo, matData);
}

void Tunnel::OnTriggerEnter(GameObject& other)
{
	if (other.GetComponent<PlayerComponent>())
	{
		Scene* currentScene = PM3D::CMoteurWindows::GetInstance().GetCurrentScene();
		MainScene* mainScene = dynamic_cast<MainScene*>(currentScene);

		if (mainScene)
		{
			mainScene->SetLight(tunnelLight);
			mainScene->SetSnow(false);
		}
	}
}

void Tunnel::OnTriggerStay(GameObject& other)
{
}

void Tunnel::OnTriggerExit(GameObject& other)
{
	if (other.GetComponent<PlayerComponent>())
	{
		Scene* currentScene = PM3D::CMoteurWindows::GetInstance().GetCurrentScene();
		MainScene* mainScene = dynamic_cast<MainScene*>(currentScene);

		if (mainScene)
		{
			mainScene->SetLight(mainScene->GetSceneLight());
			mainScene->SetSnow(true);
		}
	}
	
}
