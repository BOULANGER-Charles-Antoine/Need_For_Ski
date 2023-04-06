#include "stdafx.h"
#include "Cabine.h"

Cabine::Cabine(std::string nom, PM3D::CChargeurOBJ& chargeur) noexcept : Obstacle(nom, chargeur)
{
}

Cabine::Cabine(std::string nom, PM3D::CParametresChargement params) noexcept : Obstacle(nom, params)
{
}

Cabine::Cabine(std::string nom, PM3D::CParametresChargement params, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale_x, float scale_y, float scale_z) noexcept : Cabine(nom, params)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x - 90) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
	//scaling plus tard
}

Cabine::Cabine(std::string nom, PM3D::CParametresChargement params, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale_xyz) noexcept : Cabine(nom, params)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x - 90) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));

}
