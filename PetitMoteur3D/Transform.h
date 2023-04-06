#pragma once

const DirectX::XMVECTOR X = DirectX::XMVectorSet(1, 0, 0, 0);
const DirectX::XMVECTOR Y = DirectX::XMVectorSet(0, 1, 0, 0);
const DirectX::XMVECTOR Z = DirectX::XMVectorSet(0, 0, 1, 0);
constexpr DirectX::XMFLOAT3 X_F3 = DirectX::XMFLOAT3{ 1, 0, 0 };
constexpr DirectX::XMFLOAT3 Y_F3 = DirectX::XMFLOAT3{ 0, 1, 0 };
constexpr DirectX::XMFLOAT3 Z_F3 = DirectX::XMFLOAT3{ 0, 0, 1 };

DirectX::XMVECTOR QuaternionLookAt(const DirectX::XMVECTOR& source, const DirectX::XMVECTOR& dest, 
	const DirectX::XMVECTOR& front, const DirectX::XMVECTOR& up) noexcept;

class CTransform
{
public:
	CTransform() noexcept = default;

	/// <summary>
	/// Translate this transform.
	/// </summary>
	/// <param name="offset">Direction and distance of the translation</param>
	void Translate(const DirectX::XMVECTOR& offset) noexcept;
	/// <summary>
	/// Translate this transform.
	/// </summary>
	/// <param name="offsetX">Movement in the X axis</param>
	/// <param name="offsetY">Movement in the Y axis</param>
	/// <param name="offsetZ">Movement in the Z axis</param>
	void Translate(float offsetX, float offsetY, float offsetZ) noexcept;
	/// <summary>
	/// Translate this transform.
	/// </summary>
	/// <param name="offset">Direction and distance of the translation</param>
	void Translate(const DirectX::XMFLOAT3& offset) noexcept;

	/// <summary>
	/// Rotate this transform.
	/// </summary>
	/// <param name="quaternion">The quaternion representing the rotation</param>
	void Rotate(const DirectX::XMVECTOR& quaternion) noexcept;
	/// <summary>
	/// Rotate this transform.
	/// Angles are in radians.
	/// </summary>
	/// <param name="angleX">Rotation around the X axis</param>
	/// <param name="angleY">Rotation around the Y axis</param>
	/// <param name="angleZ">Rotation around the Z axis</param>
	void Rotate(float angleX, float angleY, float angleZ) noexcept;

	void RotateAround(const DirectX::XMVECTOR& axis, float angle) noexcept;
	void RotateAround(const DirectX::XMFLOAT3& axis, float angle) noexcept;

	/// <summary>
	/// Rotate this transform to look at the given position.
	/// </summary>
	/// <param name="lookAtPosition">Position to look at</param>
	void LookAt(const DirectX::XMVECTOR& lookAtPosition) noexcept;
	/// <summary>
	/// Rotate this transform to look at the given position.
	/// </summary>
	/// <param name="x">X coordinates of the position to look at</param>
	/// <param name="y">Y coordinates of the position to look at</param>
	/// <param name="z">Z coordinates of the position to look at</param>
	void LookAt(float x, float y, float z) noexcept;
	/// <summary>
	/// Rotate this transform to look at the given position.
	/// </summary>
	/// <param name="lookAtPosition">Position to look at</param>
	void LookAt(const DirectX::XMFLOAT3& lookAtPosition) noexcept;

	/// <summary>
	/// Rotate this transform to look in the given direction.
	/// </summary>
	/// <param name="lookToDirection">Direction to look in</param>
	void LookTo(const DirectX::XMVECTOR& lookToDirection) noexcept;
	/// <summary>
	/// Rotate this transform to look in the given direction.
	/// </summary>
	/// <param name="x">X coordinates of the direction to look in</param>
	/// <param name="y">Y coordinates of the direction to look in</param>
	/// <param name="z">Z coordinates of the direction to look in</param>
	void LookTo(float x, float y, float z) noexcept;
	/// <summary>
	/// Rotate this transform to look in the given direction.
	/// </summary>
	/// <param name="lookToDirection">Direction to look in</param>
	void LookTo(const DirectX::XMFLOAT3& lookToDirection) noexcept;

	/// <summary>
	/// Set the position of this transform.
	/// </summary>
	/// <param name="pos">New position of this transform</param>
	void SetPosition(const DirectX::XMVECTOR& pos) noexcept;
	/// <summary>
	/// Set the position of this transform.
	/// </summary>
	/// <param name="x">X coordinates of the position</param>
	/// <param name="y">Y coordinates of the position</param>
	/// <param name="z">Z coordinates of the position</param>
	void SetPosition(float x, float y, float z) noexcept;
	/// <summary>
	/// Set the position of this transform.
	/// </summary>
	/// <param name="pos">New position of this transform</param>
	void SetPosition(const DirectX::XMFLOAT3& pos) noexcept;

	/// <summary>
	/// Set the rotation of this transform.
	/// </summary>
	/// <param name="quaternion">The quaternion representing the rotation</param>
	void SetRotation(const DirectX::XMVECTOR& quaternion) noexcept;

	/// <summary>
	/// Set the scale of this transform.
	/// </summary>
	/// <param name="scale">The new scale</param>
	void SetScale(const DirectX::XMFLOAT3& scale) noexcept;
	/// <summary>
	/// Set the scale of this transform
	/// </summary>
	/// <param name="x">The scale in the x axis</param>
	/// <param name="y">The scale in the y axis</param>
	/// <param name="z">The scale in the z axis</param>
	/// <returns></returns>
	void SetScale(float x, float y, float z) noexcept;
	/// <summary>
	/// Set the scale of this transform.
	/// </summary>
	/// <param name="scale">The scale of all axis</param>
	void SetScale(float scale) noexcept;

	DirectX::XMVECTOR GetPositionVector() const noexcept;
	DirectX::XMFLOAT3 GetPositionFloat3() const noexcept;

	DirectX::XMVECTOR GetRotationQuaternion() const noexcept;

	DirectX::XMFLOAT3 GetScaleFloat3() const noexcept;

	DirectX::XMVECTOR GetForward() const noexcept;
	DirectX::XMVECTOR GetBackward() const noexcept;
	DirectX::XMVECTOR GetRight() const noexcept;
	DirectX::XMVECTOR GetLeft() const noexcept;
	DirectX::XMVECTOR GetUp() const noexcept;
	DirectX::XMVECTOR GetDown() const noexcept;

	DirectX::XMFLOAT3 GetForwardFloat3() const noexcept;
	DirectX::XMFLOAT3 GetBackwardFloat3() const noexcept;
	DirectX::XMFLOAT3 GetRightFloat3() const noexcept;
	DirectX::XMFLOAT3 GetLeftFloat3() const noexcept;
	DirectX::XMFLOAT3 GetUpFloat3() const noexcept;
	DirectX::XMFLOAT3 GetDownFloat3() const noexcept;

	DirectX::XMMATRIX GetWorldMatrix() const noexcept;

private:
	DirectX::XMVECTOR position = DirectX::XMVectorSet(0, 0, 0, 1);
	DirectX::XMVECTOR rotation = DirectX::XMQuaternionIdentity();
	DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3{ 1, 1, 1 };

	DirectX::XMVECTOR forward = Z;
	DirectX::XMVECTOR up = Y;

	/// <summary>
	/// Update the local axis of the transform
	/// </summary>
	void UpdateAxis() noexcept;
};