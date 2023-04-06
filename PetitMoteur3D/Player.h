#pragma once
#include "GameObject.h"
#include "Component.h"
#include "Time.h"
#include <string>

const inline std::string PLAYER_NAME = "Player";

struct PlayerComponent : public Component
{
	PlayerComponent(GameObject& gameObject) : Component(gameObject) { /*vide*/ }
};

class Player final : public GameObject
{
	static const inline DirectX::XMFLOAT3 REF_UP { 0, 1.0f, 0 };
	static const inline DirectX::XMFLOAT3 REF_FORWARD { -1.0f, 0, 0 };
	static const inline physx::PxVec3 FORWARD_FORCE = physx::PxVec3{ 0.984f, -0.173f, 0 };

	static constexpr const float UP_ANGLE_LIMIT = 45.0f;
	static constexpr const float FORWARD_ANGLE_LIMIT = 90.0f;

	static constexpr const float MAX_VELOCITY = 30.0f;
	static constexpr const float MIN_VELOCITY = 2.5f;

	static constexpr const float MAX_LANDING_VELOCITY = MAX_VELOCITY / 2.0f;
	static constexpr const float LANDING_VELOCITY_COEFF = 1.0f;

	static constexpr const float ACC_FORCE = 8.0f;
	static constexpr const float BRAKE_FORCE = 10.0f;
	static constexpr const float TURN_FORCE = 0.3f;
	static constexpr const float TURN_BRAKE_COEF = 0.995f;

	static constexpr const float ROTATION_SPEED = DirectX::XM_PI;
	static constexpr const float LATERAL_AIR_SPEED = 5.0f;

#ifdef _DEBUG
	static constexpr const float JUMP_ANGLE = 90.f;
	static constexpr const float JUMP_FORCE = 10.0f;
#endif

	static constexpr const float START_TIME = 0.75f;
	static constexpr const float LAND_TIME = 1.5f;
	static constexpr const float MIN_AIR_TIME = 0.25f;

	enum PlayerState
	{
		DEFAULT,
		DEAD,
		GROUNDED,
		JUMPING,
		AIR,
	};
	
	PlayerState state;

	Timer airTimer;
	Timer startTimer;
	Timer landTimer;

	float rotBackFront = 0.0f;
	float rot360 = 0.0f;

public:
	Player() noexcept;
	Player(DirectX::XMFLOAT3 position) noexcept;
	Player(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction) noexcept;
	Player(DirectX::XMVECTOR position) noexcept;
	Player(DirectX::XMVECTOR position, DirectX::XMVECTOR direction) noexcept;

	bool IsDead() const noexcept { return state == DEAD; }
	void Kill() noexcept;

#ifdef _DEBUG
	std::string GetState() const
	{
		switch (state)
		{
		case DEFAULT:
			return "Default";
		case DEAD:
			return "Dead";
		case GROUNDED:
			return "Grounded";
		case JUMPING:
			return "Jumping";
		case AIR:
			return "Air";
		}
	}
#endif

protected:
	PM3D::CObjet3D* InitMesh() override;
	physx::PxRigidActor* InitActor() override;
	void InitComponents() override;

	void OnCollisionEnter(GameObject& other) override;
	void OnCollisionStay(GameObject& other) override;
	void OnCollisionExit(GameObject& other) override;

	void OnStart() override;
	void OnUpdate() override;

private:
	void updateTimer();

	void land();
	void hitObstacle();
	void jump();

	void doMoveGround();
	void doMoveAir();

	void checkFall();
};

