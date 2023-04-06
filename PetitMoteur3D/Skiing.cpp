#include "stdafx.h"
#include "Skiing.h"
#include "DIManipulateur.h"
#include "Time.h"
#include "TerrainObject.h"
#include "Player.h"
#include "GameManager.h"

using namespace physx;
using namespace DirectX;

Skiing::Skiing(GameObject& gameObject) : Component(gameObject)
{
	//Vide
}

void Skiing::Start()
{
	PxRigidDynamic& dynamicActor = static_cast<PxRigidDynamic&>(gameObject.GetActor());
	dynamicActor.setMaxLinearVelocity(MAX_VELOCITY);

	airTime = 0.0f;
	GetTransform().LookTo(ToXMFloat3(-GLOBAL_FORWARD));
}

void Skiing::Update()
{
	if (GameManager::GetInstance().IsPlayerDead()) return;

	if (grounded)
	{
		DoMoveGround();
	}
	else
	{
		airTime += Time::GetDeltaTime();
		if (airTime > MIN_AIR_TIME)
		{
			DoMoveAir();
		}
	}
}

void Skiing::OnCollisionEnter(GameObject& other)
{
	if (GameManager::GetInstance().IsPlayerDead()) return;

	if (other.GetName() == TERRAIN_NAME)
	{
		if (airTime > MIN_AIR_TIME)
		{
			SetLandingVelocity();
		}

		if (rotBackFront > XM_PI) {
			GameManager::GetInstance().IncreaseFreestyleMultiplier(1);
		}
		else if (rotBackFront < -XM_PI) {
			GameManager::GetInstance().IncreaseFreestyleMultiplier(1);
		}
		if (rot360 > XM_PI) {
			GameManager::GetInstance().IncreaseFreestyleMultiplier(1);
		}
		else if (rot360 < -XM_PI) {
			GameManager::GetInstance().IncreaseFreestyleMultiplier(1);
		}
	}
	
	airTime = 0.0f;
	grounded = true;

	rotBackFront = 0.0f;
	rot360 = 0.0f;
}

void Skiing::OnCollisionExit(GameObject& other)
{
	if (GameManager::GetInstance().IsPlayerDead()) return;

	if (other.GetName() == TERRAIN_NAME)
	{
		airTime = 0.0f;
		grounded = false;
	}
}

void Skiing::DoMoveGround()
{
	const PM3D::CDIManipulateur& input = PM3D::CDIManipulateur::GetInstance();

	PxRigidDynamic& dynamicActor = static_cast<PxRigidDynamic&>(gameObject.GetActor());
	PxVec3 velocity = dynamicActor.getLinearVelocity();

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

	//----- Movement -----//
	//Minimum speed
	if (velocity.magnitude() < 0.1f)
	{
		velocity = GLOBAL_FORWARD.getNormalized() * MIN_VELOCITY;
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
		dynamicActor.addForce(GLOBAL_FORWARD.getNormalized() * ACC_FORCE, PxForceMode::eACCELERATION);
	}
	//Default Acceleration
	else
	{
		dynamicActor.addForce(GLOBAL_FORWARD.getNormalized() * ACC_FORCE / 3.0f, PxForceMode::eACCELERATION);
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

void Skiing::DoMoveAir()
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
	//360
	if (input.ToucheAppuyee(DIK_Q))
	{
		GetTransform().RotateAround(GetTransform().GetUp(), -actualRotationSpeed);
		rot360 -= actualRotationSpeed;
	}
	if (input.ToucheAppuyee(DIK_E))
	{
		GetTransform().RotateAround(GetTransform().GetUp(), actualRotationSpeed);
		rot360 += actualRotationSpeed;
	}

	// Score
	if (rotBackFront > 2 * XM_PI) {
		GameManager::GetInstance().IncreaseFreestyleMultiplier(1);
		rotBackFront = 0.0f;
	}
	else if (rotBackFront < -2 * XM_PI) {
		GameManager::GetInstance().IncreaseFreestyleMultiplier(1);
		rotBackFront = 0.0f;
	}
	if (rot360 > 2 * XM_PI) {
		GameManager::GetInstance().IncreaseFreestyleMultiplier(1);
		rot360 = 0.0f;
	}
	else if (rot360 < -2 * XM_PI) {
		GameManager::GetInstance().IncreaseFreestyleMultiplier(1);
		rot360 = 0.0f;
	}

	//Mouvement dans les airs
	if (input.ToucheAppuyee(DIK_D)) 
	{
		GetTransform().Translate(Z * -actualLateralSpeed);
	}
	if (input.ToucheAppuyee(DIK_A)) 
	{
		GetTransform().Translate(Z * actualLateralSpeed);
	}
}

void Skiing::SetLandingVelocity()
{
	PxRigidDynamic& dynamicActor = static_cast<PxRigidDynamic&>(gameObject.GetActor());
	PxVec3 velocity = dynamicActor.getLinearVelocity();

	float speed = (velocity.magnitude() > MAX_LANDING_VELOCITY) ? 
		MAX_LANDING_VELOCITY : velocity.magnitude();

	velocity = ToPxVec3(GetTransform().GetForward()) * speed;
	velocity.y = 0;
	dynamicActor.setLinearVelocity(-velocity);
}
