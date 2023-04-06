#pragma once
#include "StaticGameObject.h"

class Arrivee : public GameObject
{

public:
	Arrivee(std::string nom) noexcept;
	Arrivee(std::string nom,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_x, float scale_y, float scale_z) noexcept;

	Arrivee(std::string nom,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_xyz) noexcept;


protected:
	PM3D::CObjet3D* InitMesh() override { return nullptr; }
	physx::PxRigidActor* InitActor() override;

	void OnTriggerEnter(GameObject& other) override;
	void OnTriggerStay(GameObject& other) override;
	void OnTriggerExit(GameObject& other) override;
};