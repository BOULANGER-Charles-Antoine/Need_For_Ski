#include "stdafx.h"
#include "Pingouin.h"
#include "ObjetMesh.h"
#include "Skiing.h"
#include "TerrainObject.h"
#include "Obstacle.h"
#include "GameManager.h"
#include "Player.h"
#include "PhysXEngine.h"
#include "SoundManager.h"

#include <string>

using namespace std;
using namespace PM3D;
using namespace physx;

Pingouin::Pingouin(PM3D::CParametresChargement params) noexcept : GameObject(Pingouin_NAME), chargeur{ CGestionnaireDeChargeurOBJ::GetInstance().GetChargeur(params) }
{
	//Vide
}

Pingouin::Pingouin(DirectX::XMFLOAT3 position, PM3D::CParametresChargement params) noexcept : GameObject(Pingouin_NAME), chargeur{ CGestionnaireDeChargeurOBJ::GetInstance().GetChargeur(params) }
{
	transform.SetPosition(position);
}

Pingouin::Pingouin(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, PM3D::CParametresChargement params) noexcept : GameObject(Pingouin_NAME), chargeur{ CGestionnaireDeChargeurOBJ::GetInstance().GetChargeur(params) }
{
	transform.SetPosition(position);
	transform.LookTo(direction);
}

Pingouin::Pingouin(DirectX::XMVECTOR position, PM3D::CParametresChargement params) noexcept : GameObject(Pingouin_NAME), chargeur{ CGestionnaireDeChargeurOBJ::GetInstance().GetChargeur(params) }
{
	transform.SetPosition(position);
}

Pingouin::Pingouin(DirectX::XMVECTOR position, DirectX::XMVECTOR direction, PM3D::CParametresChargement params) noexcept : GameObject(Pingouin_NAME), chargeur{ CGestionnaireDeChargeurOBJ::GetInstance().GetChargeur(params) }
{
	transform.SetPosition(position);
	transform.LookTo(direction);
}

Pingouin::Pingouin(std::string nom, PM3D::CParametresChargement params, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale_xyz) : GameObject(Pingouin_NAME), chargeur{ CGestionnaireDeChargeurOBJ::GetInstance().GetChargeur(params) }
{
	transform.SetPosition(pos_x, pos_z, -pos_y);
	transform.Rotate((rot_x - 90)* (DirectX::XM_PI / 180), rot_z* (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
	transform.SetScale(scale_xyz);
}

PM3D::CObjet3D* Pingouin::InitMesh()
{
	return new PM3D::CObjetMesh(chargeur);
}

physx::PxRigidActor* Pingouin::InitActor()
{
	MaterialData matData;
	matData.staticFriction = 0.6f;
	matData.dynamicFriction = 0.6f;
	matData.restitution = 0.5f;

	RigidDynamicData actorData{ id };
	actorData.layer = Layer::Player;
	actorData.isTrigger = false;
	actorData.useGravity = true;

	actorData.mass = 0.1f;
	//actorData.angularDamping = 0.1f;
	//actorData.linearDamping = 0.0f;

	const physx::PxBoxGeometry geo{ 0.5f, 0.2f, 1.0f };
	//const PxSphereGeometry geo{ 1.0f };
	PxRigidDynamic* actor = PhysXEngine::CreateRigidDynamic(actorData, geo, matData);
	if (!actor) throw;

	return actor;
}

void Pingouin::InitComponents()
{
	
}

void Pingouin::OnStart()
{
	//SoundManager::GetInstance().createMySourceVoice("resources/sounds/Coin1.wav", "coin1" + id, 1, XAUDIO2_MAX_VOLUME_LEVEL);
}

void Pingouin::OnCollisionEnter(GameObject& other)
{
	if (other.GetComponent<PlayerComponent>())
	{
		PxRigidDynamic* dynamicActor = static_cast<PxRigidDynamic*>(actor);
		PxVec3 force = PxVec3(-1,1,0)*100;
		
		//force += PxVec3(0, 1, 0) * 1;

		//force *= 75.0f;
		dynamicActor->addForce(force);
		//dynamicActor->addTorque(PxVec3(1, 1, 1)*100);

		srand((unsigned)time(0));
		int i;
		i = (rand() % 4) + 1;
	
		string path = "resources/sounds/Coin" + to_string(i) + ".wav";
		
		SoundManager::GetInstance().playSong(path.c_str(), "coin" + id, 0, 1);
		
		GameManager::GetInstance().IncreaseFreestyleMultiplier(1);
		
	}
}

void Pingouin::OnCollisionExit(GameObject& other)
{

}

void Pingouin::OnTriggerEnter(GameObject& other)
{

}

void Pingouin::OnUpdate()
{

}


