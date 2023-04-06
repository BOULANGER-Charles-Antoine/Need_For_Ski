#include "stdafx.h"
#include "Chalet.h"
#include "Player.h"
#include "SoundManager.h"

Chalet::Chalet(std::string nom, PM3D::CChargeurOBJ& chargeur) noexcept : Obstacle(nom, chargeur)
{
}

Chalet::Chalet(std::string nom, PM3D::CParametresChargement params) noexcept : Obstacle(nom, params)
{
}

Chalet::Chalet(std::string nom, PM3D::CParametresChargement params, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale_x, float scale_y, float scale_z) noexcept : Chalet(nom, params)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x - 90) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
	this->GetTransform().SetScale(scale_x, scale_z, scale_y);
	//scaling plus tard
}

Chalet::Chalet(std::string nom, PM3D::CParametresChargement params, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale_xyz) noexcept : Chalet(nom, params)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x - 90) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
	this->GetTransform().SetScale(scale_xyz);
}

void Chalet::OnCollisionEnter(GameObject& other)
{
	if (other.GetComponent<PlayerComponent>())
	{
		SoundManager::GetInstance().playSong("resources/sounds/hitWood.wav", "hitWood", 0, 1);
	}
}