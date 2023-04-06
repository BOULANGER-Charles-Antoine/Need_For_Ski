#pragma once
#include "Component.h"
#include "PhysXEngine.h"

//Not Used

class Skiing : public Component
{
	static const inline physx::PxVec3 GLOBAL_FORWARD = physx::PxVec3{ 0.984f, -0.173f, 0 };

	static constexpr const float MAX_VELOCITY = 30.0f;
	static constexpr const float MIN_VELOCITY = 2.5f;
	static constexpr const float MAX_LANDING_VELOCITY = MAX_VELOCITY / 5.0f;

	static constexpr const float ACC_FORCE = 8.0f;
	static constexpr const float BRAKE_FORCE = 10.0f;
	static constexpr const float TURN_FORCE = 0.3f;
	static constexpr const float TURN_BRAKE_COEF = 0.995f;

	static constexpr const float ROTATION_SPEED = DirectX::XM_PI;
	static constexpr const float LATERAL_AIR_SPEED = 5.0f;

	static constexpr const float JUMP_ANGLE = 90.f;
	static constexpr const float JUMP_FORCE = 10.0f;

	static constexpr const float MIN_AIR_TIME = 0.25f;

private:
	bool grounded = false;
	float airTime = 0.0f;

	float rotBackFront = 0.0f;
	float rot360 = 0.0f;

public:
	Skiing(GameObject&);

protected:
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnCollisionEnter(GameObject& other) override;
	virtual void OnCollisionExit(GameObject& other) override;

private:
	void DoMoveGround();
	void DoMoveAir();

	void SetLandingVelocity();
};