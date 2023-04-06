#pragma once
#include "StaticGameObject.h"
#include "Light.hpp"

class Tunnel : public GameObject 
{
	std::shared_ptr<CLight> tunnelLight;

public:
	Tunnel(std::string nom, CLight* light) noexcept;
	Tunnel(std::string nom, CLight* light,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_x, float scale_y, float scale_z) noexcept;

	Tunnel(std::string nom, CLight* light,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_xyz) noexcept;
	~Tunnel();


protected:
	PM3D::CObjet3D* InitMesh() override { return nullptr; }
	physx::PxRigidActor* InitActor() override;

	void OnTriggerEnter(GameObject& other) override;
	void OnTriggerStay(GameObject& other) override;
	void OnTriggerExit(GameObject& other) override;
};