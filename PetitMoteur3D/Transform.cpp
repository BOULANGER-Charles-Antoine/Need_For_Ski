#include "stdafx.h"
#include "Transform.h"
#include <iostream>
#include <exception>

using namespace DirectX;
using namespace std;

#pragma region Statics

XMVECTOR QuaternionLookAt(const XMVECTOR& source, const XMVECTOR& dest, const XMVECTOR& front, const XMVECTOR& up) noexcept
{
	const XMVECTOR toVector = XMVector3Normalize(dest-source);

	// Compute rotation axis
	XMVECTOR rotAxis = XMVector3Normalize(XMVector3Cross(front, toVector));
	if (XMVectorGetX(XMVector3LengthSq(rotAxis)) == 0)
	{
		rotAxis = up;
	}

	// Find the angle around the rotation axis
	float dot = XMVectorGetX(XMVector3Dot(Z, toVector));
	float rotAngle = acosf(dot);

	// Convert axis angle to quaternion
	return XMQuaternionRotationAxis(rotAxis, rotAngle);
}

#pragma endregion

void CTransform::Translate(const XMVECTOR& offset) noexcept
{
	this->position += offset;
	UpdateAxis();
}

void CTransform::Translate(float offsetX, float offsetY, float offsetZ) noexcept
{
	Translate(XMVectorSet(offsetX, offsetY, offsetZ, 1.0f));
}

void CTransform::Translate(const DirectX::XMFLOAT3& offset) noexcept
{
	Translate(offset.x, offset.y, offset.z);
}

void CTransform::Rotate(const XMVECTOR& quaternion) noexcept
{
	rotation = XMQuaternionMultiply(rotation, quaternion);
	UpdateAxis();
}

void CTransform::Rotate(float angleX, float angleY, float angleZ) noexcept
{
	const XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(angleX, angleY, angleZ);
	Rotate(quaternion);
}

void CTransform::RotateAround(const DirectX::XMVECTOR& axis, float angle) noexcept
{
	const XMVECTOR quaternion = XMQuaternionRotationAxis(axis, angle);
	Rotate(quaternion);
}

void CTransform::RotateAround(const DirectX::XMFLOAT3& axis, float angle) noexcept
{
	RotateAround(XMVectorSet(axis.x, axis.y, axis.z, 0), angle);
}

void CTransform::LookAt(const XMVECTOR& lookAtPosition) noexcept
{
	// Rotation ignoring the Y axis
	const XMVECTOR flatPos = XMVectorSetY(position, 0);
	const XMVECTOR flatLookAtPos = XMVectorSetY(lookAtPosition, 0);

	const XMVECTOR firstQuat = QuaternionLookAt(flatPos, flatLookAtPos, Z, Y);
	SetRotation(firstQuat);

	const XMVECTOR targetDirection = XMVector3Normalize(lookAtPosition - position);
	const float rotAngle = XMVectorGetX(XMVector2AngleBetweenVectors(forward, targetDirection));
	if (rotAngle > 0.000001f)
	{
		RotateAround(GetRight(), rotAngle);
	}
	
}

void CTransform::LookAt(float x, float y, float z) noexcept
{
	LookAt(XMVectorSet(x, y, z, 0));
}

void CTransform::LookAt(const DirectX::XMFLOAT3& lookAtPosition) noexcept
{
	LookAt(lookAtPosition.x, lookAtPosition.y, lookAtPosition.z);
}

void CTransform::LookTo(const XMVECTOR& lookToDirection) noexcept
{
	LookAt(position + lookToDirection);
}

void CTransform::LookTo(float x, float y, float z) noexcept
{
	LookTo(XMVectorSet(x, y, z, 0));
}

void CTransform::LookTo(const DirectX::XMFLOAT3& lookToDirection) noexcept
{
	LookTo(lookToDirection.x, lookToDirection.y, lookToDirection.z);
}

#pragma region Setters

void CTransform::SetPosition(const XMVECTOR& position_) noexcept
{
	this->position = position_;
	UpdateAxis();
}

void CTransform::SetPosition(float x, float y, float z) noexcept
{
	SetPosition(XMVectorSet(x, y, z, 1.0f));
}

void CTransform::SetPosition(const DirectX::XMFLOAT3& pos) noexcept
{
	SetPosition(pos.x, pos.y, pos.z);
}

void CTransform::SetRotation(const XMVECTOR& quaternion) noexcept
{
	this->rotation = quaternion;
	UpdateAxis();
}

void CTransform::SetScale(const XMFLOAT3& scale) noexcept
{
	this->scale = scale;
}

void CTransform::SetScale(float x, float y, float z) noexcept
{
	this->scale = XMFLOAT3{ x, y, z };
}

#pragma endregion

#pragma region Getter

void CTransform::SetScale(float scale) noexcept
{
	this->scale = XMFLOAT3{ scale, scale, scale };
}

XMVECTOR CTransform::GetPositionVector() const noexcept
{
	return position;
}

XMFLOAT3 CTransform::GetPositionFloat3() const noexcept
{
	float x = XMVectorGetX(position);
	float y = XMVectorGetY(position);
	float z = XMVectorGetZ(position);

	return XMFLOAT3(x, y, z);
}

DirectX::XMVECTOR CTransform::GetRotationQuaternion() const noexcept
{
	return rotation;
}

DirectX::XMFLOAT3 CTransform::GetScaleFloat3() const noexcept
{
	return scale;
}

XMVECTOR CTransform::GetForward() const noexcept
{
	return forward;
}

XMVECTOR CTransform::GetBackward() const noexcept
{
	return -GetForward();
}

XMVECTOR CTransform::GetRight() const noexcept
{
	return XMVector3Cross(GetUp(), GetForward());
}

XMVECTOR CTransform::GetLeft() const noexcept
{
	return -GetRight();
}

XMVECTOR CTransform::GetUp() const noexcept
{
	return up;
}

XMVECTOR CTransform::GetDown() const noexcept
{
	return -GetUp();
}

DirectX::XMFLOAT3 CTransform::GetForwardFloat3() const noexcept
{
	float x = XMVectorGetX(GetForward());
	float y = XMVectorGetY(GetForward());
	float z = XMVectorGetZ(GetForward());

	return XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 CTransform::GetBackwardFloat3() const noexcept
{
	float x = XMVectorGetX(GetBackward());
	float y = XMVectorGetY(GetBackward());
	float z = XMVectorGetZ(GetBackward());

	return XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 CTransform::GetRightFloat3() const noexcept
{
	float x = XMVectorGetX(GetRight());
	float y = XMVectorGetY(GetRight());
	float z = XMVectorGetZ(GetRight());

	return XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 CTransform::GetLeftFloat3() const noexcept
{
	float x = XMVectorGetX(GetLeft());
	float y = XMVectorGetY(GetLeft());
	float z = XMVectorGetZ(GetLeft());

	return XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 CTransform::GetUpFloat3() const noexcept
{
	float x = XMVectorGetX(GetUp());
	float y = XMVectorGetY(GetUp());
	float z = XMVectorGetZ(GetUp());

	return XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 CTransform::GetDownFloat3() const noexcept
{
	float x = XMVectorGetX(GetDown());
	float y = XMVectorGetY(GetDown());
	float z = XMVectorGetZ(GetDown());

	return XMFLOAT3(x, y, z);
}

DirectX::XMMATRIX CTransform::GetWorldMatrix() const noexcept
{
	const XMMATRIX matTranslation = XMMatrixTranslationFromVector(position);
	const XMMATRIX matRotation = XMMatrixRotationQuaternion(rotation);
	const XMMATRIX matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	
	XMMATRIX matWorld = XMMatrixMultiply(matScale, matRotation);
	matWorld = XMMatrixMultiply(matWorld, matTranslation);
	return matWorld;
}

#pragma endregion

#pragma region Privates

void CTransform::UpdateAxis() noexcept
{
	forward = XMVector3Normalize(XMVector3Rotate(Z, rotation));
	up = XMVector3Normalize(XMVector3Rotate(Y, rotation));
}

#pragma endregion


