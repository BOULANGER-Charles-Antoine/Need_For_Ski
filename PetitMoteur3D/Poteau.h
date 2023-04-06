#pragma once
#include "Obstacle.h"

class Poteau : public Obstacle {
public:
	Poteau(std::string nom, PM3D::CChargeurOBJ& chargeur) noexcept;
	Poteau(std::string nom, PM3D::CParametresChargement params) noexcept;
	Poteau(std::string nom, PM3D::CParametresChargement params,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_x, float scale_y, float scale_z) noexcept;

	Poteau(std::string nom, PM3D::CParametresChargement params,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_xyz) noexcept;
	void OnCollisionEnter(GameObject& other) override;
};