#pragma once
#include "Transform.h"
#include "dispositif.h"
#include <memory>

class GameObject;

class CCamera
{
public:
	CCamera() noexcept;
	CCamera(DirectX::XMVECTOR position_in, DirectX::XMVECTOR direction_in) noexcept;
	CCamera(DirectX::XMFLOAT3 position_in, DirectX::XMFLOAT3 direction_in) noexcept;

	virtual void Start() { /*� impl�menter*/ };
	virtual void Update() { /*� impl�menter*/ };

	//----- Getter -----//
	virtual CTransform& GetTransform() noexcept;
	
	CTransform GetTransform() const noexcept;
	DirectX::XMMATRIX GetMatView() const noexcept;
	DirectX::XMMATRIX GetMatProj() const noexcept;
	DirectX::XMMATRIX GetMatViewProj() const noexcept;

	float GetFieldOfView() const noexcept;
	float GetAspectRatio() const noexcept;
	float GetFarPlane() const noexcept;
	float GetClosePlane() const noexcept;
	PM3D::CDispositif* GetDispositif() const noexcept;

	//----- Setter -----//
	void SetFieldOfView(float) noexcept;
	void SetAspectRatio(float) noexcept;
	void SetFarPlane(float) noexcept;
	void SetClosePlane(float) noexcept;

protected:
	float champDeVision = DirectX::XM_PI / 4;
	float ratioDAspect = 1.0f;
	float planRapproche = 0.05f;
	float planEloigne = 99999.0f;

	CTransform transform{};

private:
	PM3D::CDispositif* pDispositif;
};

class CFreeCamera : public CCamera
{
	float speed = 20.0f;
	float rotationSpeed = DirectX::XM_PI / 2.0f;

public:
	CFreeCamera() noexcept;
	CFreeCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR direction) noexcept;
	CFreeCamera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction) noexcept;

	virtual void Update() override;

	//----- Getter -----//
	float GetSpeed() const noexcept;
	float GetRotationSpeed() const noexcept;

	//----- Setter -----//
	void SetSpeed(float) noexcept;
	void SetRotationSpeed(float) noexcept;
};

class CFollowCamera : public CCamera
{
	std::weak_ptr<GameObject> target;
	DirectX::XMVECTOR offset;

public:
	CFollowCamera(std::weak_ptr<GameObject> target_in, DirectX::XMVECTOR offset_in) noexcept;
	CFollowCamera(std::weak_ptr<GameObject> target_in, DirectX::XMFLOAT3 offset_in) noexcept;

	virtual void Start() override;
	virtual void Update() override;

	//----- Getter -----//
	std::weak_ptr<GameObject> GetTarget() const noexcept;
	DirectX::XMVECTOR GetOffset() const noexcept;

	//----- Setter -----//
	void SetTarget(std::weak_ptr<GameObject>) noexcept;
	void SetOffset(DirectX::XMVECTOR) noexcept;
	void SetOffset(DirectX::XMFLOAT3) noexcept;

private:
	void follow() noexcept;
};

class TPSCamera : public CFollowCamera
{
	static constexpr const DirectX::XMFLOAT3 POSITION_OFFSET{ -15.0f, 7.0f, 0 };
	static constexpr const DirectX::XMFLOAT3 LOOK_OFFSET{ 0, 2.0f, 0 };

public:
	TPSCamera() noexcept;

	void Start() override;
	void Update() override;
};

class FPSCamera : public CFollowCamera
{
	static constexpr const DirectX::XMFLOAT3 POSITION_OFFSET{ 0, 2, 0 };

public:
	FPSCamera() noexcept;

	void Start() override;
	void Update() override;
};