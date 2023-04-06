#include "stdafx.h"
#include "Camera.h"
#include "MoteurWindows.h"
#include "GameManager.h"
#include "GameObject.h"

using namespace DirectX;
using namespace PM3D;

#pragma region CCamera

CCamera::CCamera() noexcept 
	: pDispositif{ CMoteurWindows::GetInstance().GetDispositif() }
{
	ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());
}

CCamera::CCamera(DirectX::XMVECTOR position_in, DirectX::XMVECTOR direction_in) noexcept
	: pDispositif{ CMoteurWindows::GetInstance().GetDispositif() }
{
	ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());

	transform.SetPosition(position_in);
	transform.LookTo(direction_in);
}
CCamera::CCamera(DirectX::XMFLOAT3 position_in, DirectX::XMFLOAT3 direction_in) noexcept
	: pDispositif{ CMoteurWindows::GetInstance().GetDispositif() }
{
	ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());

	transform.SetPosition(position_in);
	transform.LookTo(direction_in);
}

CTransform& CCamera::GetTransform() noexcept
{
	return transform;
}

CTransform CCamera::GetTransform() const noexcept
{
	return transform;
}

DirectX::XMMATRIX CCamera::GetMatView() const noexcept
{
	XMMATRIX matView = XMMatrixLookAtLH(
		transform.GetPositionVector(),
		transform.GetPositionVector() + transform.GetForward(),
		transform.GetUp()
	);

	return matView;
}

DirectX::XMMATRIX CCamera::GetMatProj() const noexcept
{
	XMMATRIX matProj = XMMatrixPerspectiveFovLH(champDeVision, ratioDAspect, planRapproche, planEloigne);

	return matProj;
}

DirectX::XMMATRIX CCamera::GetMatViewProj() const  noexcept
{
	XMMATRIX matViewProj = GetMatView() * GetMatProj();

	return matViewProj;
}

float CCamera::GetFieldOfView() const noexcept
{
	return champDeVision;
}

float CCamera::GetAspectRatio() const noexcept
{
	return ratioDAspect;
}

PM3D::CDispositif* CCamera::GetDispositif() const noexcept {
	return pDispositif;
}

float CCamera::GetFarPlane() const noexcept
{
	return planEloigne;
}

float CCamera::GetClosePlane() const noexcept
{
	return planRapproche;
}

void CCamera::SetFieldOfView(float fov) noexcept
{
	champDeVision = fov;
}

void CCamera::SetAspectRatio(float aspectRatio) noexcept
{
	ratioDAspect = aspectRatio;
}

void CCamera::SetFarPlane(float farPlane) noexcept
{
	planEloigne = farPlane;
}

void CCamera::SetClosePlane(float closePlane) noexcept
{
	planRapproche = closePlane;
}

#pragma endregion

#pragma region CFreeCamera

CFreeCamera::CFreeCamera() noexcept : CCamera()
{
	//Vide
}

CFreeCamera::CFreeCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR direction) noexcept
	: CCamera(position, direction)
{
	//Vide
}

CFreeCamera::CFreeCamera(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction) noexcept
	: CCamera(position, direction)
{
	//Vide
}

void CFreeCamera::Update()
{
	SetAspectRatio(static_cast<float>(GetDispositif()->GetLargeur()) / static_cast<float>(GetDispositif()->GetHauteur()));
	CDIManipulateur& rGestionnaireDeSaisie = CDIManipulateur::GetInstance();

	// ### TRANSLATION ###
	float actualSpeed = speed * Time::GetDeltaTime();
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_LSHIFT))
	{
		actualSpeed *= 10;
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_LCONTROL))
	{
		actualSpeed /= 10;
	}

	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_W))
	{
		transform.Translate(transform.GetForward() * actualSpeed);
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_S))
	{
		transform.Translate(transform.GetBackward() * actualSpeed);
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_A))
	{
		transform.Translate(transform.GetLeft() * actualSpeed);
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_D))
	{
		transform.Translate(transform.GetRight() * actualSpeed);
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_Q))
	{
		transform.Translate(transform.GetUp() * actualSpeed);
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_E))
	{
		transform.Translate(transform.GetDown() * actualSpeed);
	}

	// ### ROTATION ###
	float pitchOffset = 0.0f;
	float yawOffset = 0.0f;

	float actualRotationSpeed = rotationSpeed * Time::GetDeltaTime();
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_UP))
	{
		pitchOffset -= actualRotationSpeed;
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_DOWN))
	{
		pitchOffset += actualRotationSpeed;
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_RIGHT))
	{
		yawOffset += actualRotationSpeed;
	}
	if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_LEFT))
	{
		yawOffset -= actualRotationSpeed;
	}

	if (rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80)
	{
		pitchOffset = Time::GetDeltaTime() * (rGestionnaireDeSaisie.EtatSouris().lY / 2.0f);
		yawOffset = Time::GetDeltaTime() * (rGestionnaireDeSaisie.EtatSouris().lX / 2.0f);
	}

	transform.RotateAround(transform.GetRight(), pitchOffset);
	transform.RotateAround(Y, yawOffset);
}

float CFreeCamera::GetSpeed() const noexcept
{
	return speed;
}

float CFreeCamera::GetRotationSpeed() const noexcept
{
	return rotationSpeed;
}

void CFreeCamera::SetSpeed(float _speed) noexcept
{
	speed = _speed;
}

void CFreeCamera::SetRotationSpeed(float _rotationSpeed) noexcept
{
	rotationSpeed = _rotationSpeed;
}

#pragma endregion

#pragma region CFollowCamera

CFollowCamera::CFollowCamera(std::weak_ptr<GameObject> target_in, DirectX::XMVECTOR offset_in) noexcept
	: CCamera(), target{target_in}, offset{offset_in}
{
	follow();
}

CFollowCamera::CFollowCamera(std::weak_ptr<GameObject> target_in, DirectX::XMFLOAT3 offset_in) noexcept
	: CCamera(), target{target_in}, offset{DirectX::XMVectorSet(offset_in.x, offset_in.y, offset_in.z, 0)}
{
	follow();
}

void CFollowCamera::Start()
{
	follow();
}

void CFollowCamera::Update()
{
	SetAspectRatio(static_cast<float>(GetDispositif()->GetLargeur()) / static_cast<float>(GetDispositif()->GetHauteur()));
	follow();
}

std::weak_ptr<GameObject> CFollowCamera::GetTarget() const noexcept
{
	return target;
}

DirectX::XMVECTOR CFollowCamera::GetOffset() const noexcept
{
	return DirectX::XMVECTOR();
}

void CFollowCamera::SetTarget(std::weak_ptr<GameObject> _target) noexcept
{
	target = _target;
	follow();
}

void CFollowCamera::SetOffset(DirectX::XMVECTOR _offset) noexcept
{
	offset = _offset;
	Start();
}

void CFollowCamera::SetOffset(DirectX::XMFLOAT3 _offset) noexcept
{
	offset = DirectX::XMVectorSet(_offset.x, _offset.y, _offset.z, 0);
	Start();
}

void CFollowCamera::follow() noexcept
{
	if (target.expired()) return;

	CTransform targetTransform = target.lock()->GetTransform();
	XMVECTOR position = targetTransform.GetPositionVector() + offset;
	transform.SetPosition(position);
}

#pragma endregion

TPSCamera::TPSCamera() noexcept : CFollowCamera(std::weak_ptr<Player>(), POSITION_OFFSET)
{
	//Vide
}

void TPSCamera::Start()
{
	if (!GameManager::GetInstance().IsInLevel()) return;

	SetTarget(CMoteurWindows::GetInstance().GetCurrentScene()->GetPlayer());
	
	CFollowCamera::Start();
	GetTransform().LookAt(GetTarget().lock()->GetTransform().GetPositionVector() + ToXMVector(LOOK_OFFSET));
}

void TPSCamera::Update()
{
	if (!GameManager::GetInstance().IsInLevel()) return;
	if (GameManager::GetInstance().IsPlayerDead()) return;

	CFollowCamera::Update();
}

FPSCamera::FPSCamera() noexcept : CFollowCamera(std::weak_ptr<Player>(), POSITION_OFFSET)
{
	//Vide
}

void FPSCamera::Start()
{
	if (!GameManager::GetInstance().IsInLevel()) return;

	SetTarget(CMoteurWindows::GetInstance().GetCurrentScene()->GetPlayer());

	CFollowCamera::Start();
	GetTransform().LookAt(GetTarget().lock()->GetTransform().GetForward());
}

void FPSCamera::Update()
{
	if (!GameManager::GetInstance().IsInLevel()) return;
	if (GameManager::GetInstance().IsPlayerDead()) return;

	CFollowCamera::Update();

	std::weak_ptr<GameObject> playerTarget = GetTarget();
	if (!playerTarget.expired())
	{
		DirectX::XMVECTOR playerForward = -playerTarget.lock()->GetTransform().GetForward();
		playerForward = DirectX::XMVectorSetY(playerForward, 0.0f);

		GetTransform().LookTo(playerForward);
	}
}
