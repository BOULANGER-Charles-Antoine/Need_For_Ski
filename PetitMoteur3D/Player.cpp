#include "stdafx.h"
#include "Player.h"
#include "ObjetMesh.h"
#include "Skiing.h"
#include "TerrainObject.h"
#include "Obstacle.h"
#include "GameManager.h"
#include "SoundManager.h"

#include <string>

using namespace std;
using namespace PM3D;
using namespace physx;
using namespace DirectX;

Player::Player() noexcept : GameObject(PLAYER_NAME)
{
	//Vide
}

Player::Player(DirectX::XMFLOAT3 position) noexcept : GameObject(PLAYER_NAME)
{
	transform.SetPosition(position);
}

Player::Player(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction) noexcept : GameObject(PLAYER_NAME)
{
	transform.SetPosition(position);
	transform.LookTo(direction);
}

Player::Player(DirectX::XMVECTOR position) noexcept : GameObject(PLAYER_NAME)
{
	transform.SetPosition(position);
}

Player::Player(DirectX::XMVECTOR position, DirectX::XMVECTOR direction) noexcept : GameObject(PLAYER_NAME)
{
	transform.SetPosition(position);
	transform.LookTo(direction);
}

void Player::Kill() noexcept
{
	if (!IsDead())
	{
		state = DEAD;

		SoundManager::GetInstance().playSong("resources/sounds/mort.wav", "mort", 0, 1);

		PxRigidDynamic& dynActor = static_cast<PxRigidDynamic&>(GetActor());
		dynActor.setAngularDamping(1.0f);
		//dynActor.setMaxAngularVelocity(500);	//� voir
	}
}

PM3D::CObjet3D* Player::InitMesh()
{
	PM3D::CParametresChargement params;
	params.NomChemin = "./modeles/player/";
	params.bMainGauche = true;
	params.NomFichier = "player.obj";

	PM3D::CChargeurOBJ chargeur;
	chargeur.Chargement(params);

	return new PM3D::CObjetMesh(chargeur);
}

physx::PxRigidActor* Player::InitActor()
{
	MaterialData matData;
	matData.staticFriction = 0.0f;
	matData.dynamicFriction = 0.3f;
	matData.restitution = 0.0f;

	RigidDynamicData actorData{id};
	actorData.layer = Layer::Player;
	actorData.isTrigger = false;
	actorData.useGravity = true;

	actorData.mass = 5.0f;
	actorData.angularDamping = 50.0f;
	actorData.linearDamping = 0.0f;

	//const physx::PxBoxGeometry geo{ 0.5f, 0.2f, 1.0f };
	const PxSphereGeometry geo{ 1.0f };
	PxRigidDynamic* actor = PhysXEngine::CreateRigidDynamic(actorData, geo, matData);
	if (!actor) throw;

	return actor;
}

void Player::InitComponents()
{
	components.push_back(make_shared<PlayerComponent>(*this));
}

void Player::OnStart()
{
	state = DEFAULT;
	startTimer.Start(START_TIME);

	PxRigidDynamic& dynActor = static_cast<PxRigidDynamic&>(GetActor());
	dynActor.setMaxLinearVelocity(MAX_VELOCITY);
	//dynActor.setMaxAngularVelocity(0);	//� voir
	dynActor.setAngularDamping(50.0f);
	dynActor.setLinearVelocity(PxVec3{ 0, 0, 0 });
	dynActor.setAngularVelocity(PxVec3{ 0, 0, 0 });
}

void Player::OnUpdate()
{
	if (IsDead()) return;

	//----- Timers -----//
	updateTimer();

	if (startTimer.Done())
	{
		startTimer.Stop();
	}
	if (landTimer.Done())
	{
		landTimer.Stop();
	}

	if (state == GROUNDED)
	{
		doMoveGround();
	}
	else if(state == JUMPING && airTimer.Done())
	{
		state = AIR;
		doMoveAir();
	}
	else if (state == AIR)
	{
		doMoveAir();
	}
}

void Player::OnCollisionEnter(GameObject& other)
{
	if (IsDead()) return;

	const bool inAir = (state == AIR);

	//Collision avec un obstacle
	if (other.GetComponent<ObstacleComponent>())
	{
		airTimer.Stop();
		state = GROUNDED;

		if (inAir)
		{
			if(!startTimer.IsRunning()) Kill();
		}
		else
		{
			hitObstacle();
		}
	}
	//Collision avec le terrain
	else if (other.GetComponent<TerrainComponent>())
	{
		airTimer.Stop();
		landTimer.Start(LAND_TIME);
		state = GROUNDED;

		if (inAir)
		{
			land();
		}
	}
}

void Player::OnCollisionStay(GameObject& other)
{
	if (IsDead()) return;

	if (state == GROUNDED) checkFall();
}

void Player::OnCollisionExit(GameObject& other)
{
	if (IsDead()) return;

	if (other.GetComponent<TerrainComponent>())
	{
		if (landTimer.IsRunning())	airTimer.Start(MIN_AIR_TIME + LAND_TIME);
		else						airTimer.Start(MIN_AIR_TIME);

		state = JUMPING;

		jump();
	}
}

#pragma region Privates

void Player::updateTimer()
{
	if (landTimer.IsRunning()) landTimer.Update();
	if (airTimer.IsRunning()) airTimer.Update();
	if (startTimer.IsRunning()) startTimer.Update();
}

void Player::land()
{
	//----- Reset figures -----//
	rot360 = 0;
	rotBackFront = 0;

	//----- Check orientation -----//
	const XMVECTOR refForward = ToXMVector(REF_FORWARD);
	const XMVECTOR forward = GetTransform().GetForward();

	const float forwardAngle = ToDeg(XMVectorGetX(XMVector3AngleBetweenVectors(refForward, forward)));
	if (forwardAngle > FORWARD_ANGLE_LIMIT)
	{
		if (!startTimer.IsRunning()) Kill();
	}

	checkFall();

	//----- Set landing velocity -----//
	PxRigidDynamic& dynamicActor = static_cast<PxRigidDynamic&>(GetActor());
	PxVec3 velocity = dynamicActor.getLinearVelocity();

	float speed = (velocity.magnitude() > MAX_LANDING_VELOCITY) ? MAX_LANDING_VELOCITY : velocity.magnitude();
	speed *= (FORWARD_ANGLE_LIMIT - forwardAngle) / FORWARD_ANGLE_LIMIT;
	speed *= LANDING_VELOCITY_COEFF;

	velocity = ToPxVec3(GetTransform().GetForward()) * speed;
	velocity.y = -0.1;
	dynamicActor.setLinearVelocity(-velocity);
}

void Player::hitObstacle()
{
	GameManager::GetInstance().ResetFreestyleMultiplier();
}

void Player::jump()
{
	rot360 = 0;
	rotBackFront = 0;
}

void Player::doMoveGround()
{
	const PM3D::CDIManipulateur& input = PM3D::CDIManipulateur::GetInstance();

	PxRigidDynamic& dynamicActor = static_cast<PxRigidDynamic&>(GetActor());
	PxVec3 velocity = dynamicActor.getLinearVelocity();

#ifdef _DEBUG
	//----- Jumping -----//
	if (input.ToucheAppuyee(DIK_SPACE))
	{
		XMVECTOR flatVelocity = XMVectorSet(velocity.x, 0, velocity.z, 1);

		const XMVECTOR rotationAxis = XMVector3Cross(flatVelocity, Y);
		const XMVECTOR rotation = XMQuaternionRotationAxis(rotationAxis, JUMP_ANGLE);
		flatVelocity = XMVector3Rotate(flatVelocity, rotation);

		const PxVec3 jumpVelocity = ToPxVec3(flatVelocity).getNormalized() * velocity.magnitude() * JUMP_FORCE;
		dynamicActor.setLinearVelocity(jumpVelocity);

		return;		//Stop movement after jump
	}
	//----- End -----//
#endif

	//----- Movement -----//
	//Minimum speed
	if (velocity.magnitude() < 0.1f)
	{
		velocity = FORWARD_FORCE.getNormalized() * MIN_VELOCITY;
	}
	else if (velocity.magnitude() < MIN_VELOCITY)
	{
		velocity *= (MIN_VELOCITY / velocity.magnitude());
	}

	//Braking
	if (input.ToucheAppuyee(DIK_S))
	{
		dynamicActor.addForce(-velocity.getNormalized() * BRAKE_FORCE, PxForceMode::eACCELERATION);
	}

	//Acceleration
	if (input.ToucheAppuyee(DIK_W))
	{
		dynamicActor.addForce(FORWARD_FORCE.getNormalized() * ACC_FORCE, PxForceMode::eACCELERATION);
	}
	//Default Acceleration
	else
	{
		dynamicActor.addForce(FORWARD_FORCE.getNormalized() * ACC_FORCE / 3.0f, PxForceMode::eACCELERATION);
	}

	//Turning
	if (input.ToucheAppuyee(DIK_A))
	{
		const PxVec3 dir = (velocity + PxVec3{ 0, 0, TURN_FORCE }).getNormalized();
		const float magnitude = velocity.magnitude() * TURN_BRAKE_COEF;

		velocity = dir * magnitude;
	}
	else if (input.ToucheAppuyee(DIK_D))
	{
		const PxVec3 dir = (velocity + PxVec3{ 0, 0, -TURN_FORCE }).getNormalized();
		const float magnitude = velocity.magnitude() * TURN_BRAKE_COEF;

		velocity = dir * magnitude;
	}

	//Set velocity
	dynamicActor.setLinearVelocity(velocity);
	//----- End -----//

	//----- Look in the direction of the movement (don't touch) -----//
	GetTransform().LookTo(-ToXMVector(velocity.getNormalized()));					//Look at direction
	GetTransform().RotateAround(GetTransform().GetRight(), -XM_PI / 12.0f);			//Correction
}

void Player::doMoveAir()
{
	const PM3D::CDIManipulateur& input = PM3D::CDIManipulateur::GetInstance();

	const float actualRotationSpeed = ROTATION_SPEED * Time::GetDeltaTime();
	const float actualLateralSpeed = LATERAL_AIR_SPEED * Time::GetDeltaTime();

	//Flip
	if (input.ToucheAppuyee(DIK_S))
	{
		GetTransform().RotateAround(GetTransform().GetRight(), actualRotationSpeed);
		rotBackFront += actualRotationSpeed;
	}
	if (input.ToucheAppuyee(DIK_W))
	{
		GetTransform().RotateAround(GetTransform().GetRight(), -actualRotationSpeed);
		rotBackFront -= actualRotationSpeed;
	}
	//Mouvement dans les airs
	if (input.ToucheAppuyee(DIK_LSHIFT))
	{
		if (input.ToucheAppuyee(DIK_D))
		{
			GetTransform().RotateAround(GetTransform().GetUp(), actualRotationSpeed);
			rot360 += actualRotationSpeed;

			GetTransform().Translate(Z * -actualLateralSpeed);
		}
		if (input.ToucheAppuyee(DIK_A))
		{
			GetTransform().RotateAround(GetTransform().GetUp(), -actualRotationSpeed);
			rot360 -= actualRotationSpeed;

			GetTransform().Translate(Z * actualLateralSpeed);
		}
	}
	//360
	else
	{
		if (input.ToucheAppuyee(DIK_D))
		{
			GetTransform().RotateAround(GetTransform().GetUp(), actualRotationSpeed);
			rot360 += actualRotationSpeed;
		}
		if (input.ToucheAppuyee(DIK_A))
		{
			GetTransform().RotateAround(GetTransform().GetUp(), -actualRotationSpeed);
			rot360 -= actualRotationSpeed;
		}
	}

	// Score
	if (rotBackFront > 2 * XM_PI || rotBackFront < -2 * XM_PI)
	{
		GameManager::GetInstance().IncreaseFreestyleMultiplier(1);
		rotBackFront = 0.0f;
	}
	if (rot360 > 2 * XM_PI || rot360 < -2 * XM_PI)
	{
		GameManager::GetInstance().IncreaseFreestyleMultiplier(1);
		rot360 = 0.0f;
	}
}

void Player::checkFall()
{
	const XMVECTOR refUp = ToXMVector(REF_UP);
	const XMVECTOR up = GetTransform().GetUp();

	const float upAngle = ToDeg(XMVectorGetX(XMVector3AngleBetweenVectors(refUp, up)));
	if (upAngle > UP_ANGLE_LIMIT)
	{
		if (!startTimer.IsRunning()) Kill();
	}
}

#pragma endregion