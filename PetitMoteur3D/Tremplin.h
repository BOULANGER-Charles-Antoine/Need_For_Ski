#pragma once
#include "StaticGameObject.h"

class Tremplin : public StaticGameObject {
public:
	Tremplin(std::string nom, PM3D::CChargeurOBJ& chargeur) noexcept;
	Tremplin(std::string nom, PM3D::CParametresChargement params) noexcept;
	Tremplin(std::string nom, PM3D::CParametresChargement params,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_x, float scale_y, float scale_z) noexcept;

	Tremplin(std::string nom, PM3D::CParametresChargement params,
		float pos_x, float pos_y, float pos_z,
		float rot_x, float rot_y, float rot_z,
		float scale_xyz) noexcept;

	RigidActorData GetActorData() const override;

	MaterialData GetMaterialData() const override;

protected:
	void OnCollisionEnter(GameObject& other) override;
};