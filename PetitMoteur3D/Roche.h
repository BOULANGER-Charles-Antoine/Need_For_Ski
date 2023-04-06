#pragma once
#include "Obstacle.h"

class Roche : public Obstacle {
public:
	Roche(std::string nom, PM3D::CChargeurOBJ& chargeur) noexcept;
	Roche(std::string nom, PM3D::CParametresChargement params) noexcept;
	Roche(std::string nom, PM3D::CParametresChargement params,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_x, float scale_y, float scale_z) noexcept;

	Roche(std::string nom, PM3D::CParametresChargement params,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_xyz) noexcept;

	void OnCollisionEnter(GameObject& other) override;

};