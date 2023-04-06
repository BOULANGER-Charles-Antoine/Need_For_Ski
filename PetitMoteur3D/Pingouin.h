//#pragma once
//#include "Obstacle.h"
//
//class Pingouin : public Obstacle {
//public:
//	Pingouin(std::string nom, PM3D::CChargeurOBJ& chargeur) noexcept;
//	Pingouin(std::string nom, PM3D::CParametresChargement params) noexcept;
//	Pingouin(std::string nom, PM3D::CParametresChargement params,
//		float pos_x, float pos_y, float pos_z,
//		float rot_x, float rot_y, float rot_z,
//		float scale_x, float scale_y, float scale_z) noexcept;
//
//	Pingouin(std::string nom, PM3D::CParametresChargement params,
//		float pos_x, float pos_y, float pos_z,
//		float rot_x, float rot_y, float rot_z,
//		float scale_xyz) noexcept;
//
//};

#pragma once
#include "GameObject.h"
#include <string>
#include "GestionnaireDeChargeur.h"

const inline std::string Pingouin_NAME = "Pingouin";

class Pingouin final : public GameObject
{

	const PM3D::CChargeurOBJ& chargeur;


public:
	Pingouin(PM3D::CParametresChargement params) noexcept;
	Pingouin(DirectX::XMFLOAT3 position, PM3D::CParametresChargement params) noexcept;
	Pingouin(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, PM3D::CParametresChargement params) noexcept;
	Pingouin(DirectX::XMVECTOR position, PM3D::CParametresChargement params) noexcept;
	Pingouin(DirectX::XMVECTOR position, DirectX::XMVECTOR direction, PM3D::CParametresChargement params) noexcept;
	Pingouin(std::string nom, PM3D::CParametresChargement params,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_xyz);

protected:
	PM3D::CObjet3D* InitMesh() override;
	physx::PxRigidActor* InitActor() override;
	void InitComponents() override;

	void OnStart() override;

	void OnCollisionEnter(GameObject& other) override;
	void OnCollisionExit(GameObject& other) override;
	void OnTriggerEnter(GameObject& other) override;

	void OnUpdate() override;
};

