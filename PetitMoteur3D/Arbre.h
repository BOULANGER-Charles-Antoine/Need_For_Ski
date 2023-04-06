#pragma once
#include "Obstacle.h"

class Arbre : public Obstacle {
public:
	Arbre(std::string nom, PM3D::CChargeurOBJ& chargeur) noexcept;
	Arbre(std::string nom, PM3D::CParametresChargement params) noexcept;
	Arbre(std::string nom, PM3D::CParametresChargement params, 
		float pos_x, float pos_y, float pos_z, 
		float rot_x, float rot_y, float rot_z, 
		float scale_x, float scale_y, float scale_z) noexcept;

	Arbre(std::string nom, PM3D::CParametresChargement params,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_xyz) noexcept;

	void OnCollisionEnter(GameObject& other) override;
};