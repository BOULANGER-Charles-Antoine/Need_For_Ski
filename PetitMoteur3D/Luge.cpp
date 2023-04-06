#include "stdafx.h"
#include "Luge.h"
#include "Player.h"
#include "SoundManager.h"

Luge::Luge(std::string nom, PM3D::CChargeurOBJ& chargeur) noexcept : Obstacle(nom, chargeur)
{
}

Luge::Luge(std::string nom, PM3D::CParametresChargement params) noexcept : Obstacle(nom, params)
{
}

Luge::Luge(std::string nom, PM3D::CParametresChargement params, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale_x, float scale_y, float scale_z) noexcept : Luge(nom, params)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
	//scaling plus tard
}

Luge::Luge(std::string nom, PM3D::CParametresChargement params, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale_xyz) noexcept : Luge(nom, params)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
}
void Luge::OnCollisionEnter(GameObject& other)
{
	if (other.GetComponent<PlayerComponent>())
	{
		SoundManager::GetInstance().playSong("resources/sounds/hitStick.wav", "hitStick", 0, 1);
	}
}