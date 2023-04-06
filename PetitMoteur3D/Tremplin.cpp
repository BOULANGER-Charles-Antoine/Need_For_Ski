#include "stdafx.h"
#include "Tremplin.h"
#include "Player.h"
#include "SoundManager.h"

Tremplin::Tremplin(std::string nom, PM3D::CChargeurOBJ& chargeur) noexcept : StaticGameObject(nom, chargeur)
{
}

Tremplin::Tremplin(std::string nom, PM3D::CParametresChargement params) noexcept : StaticGameObject(nom, params)
{
}

Tremplin::Tremplin(std::string nom, PM3D::CParametresChargement params, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale_x, float scale_y, float scale_z) noexcept : Tremplin(nom, params)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x - 90) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
	this->GetTransform().SetScale(scale_x, scale_y, scale_z);
	//scaling plus tard
}

Tremplin::Tremplin(std::string nom, PM3D::CParametresChargement params, float pos_x, float pos_y, float pos_z, float rot_x, float rot_y, float rot_z, float scale_xyz) noexcept : Tremplin(nom, params)
{
	this->GetTransform().SetPosition(pos_x, pos_z, -pos_y);
	this->GetTransform().Rotate((rot_x - 90) * (DirectX::XM_PI / 180), rot_z * (DirectX::XM_PI / 180), -rot_y * (DirectX::XM_PI / 180));
	this->GetTransform().SetScale(scale_xyz);
}

RigidActorData Tremplin::GetActorData() const
{
	RigidActorData data{ id };
	data.layer = Layer::Terrain;
	data.useGravity = false;

	MaterialData matData;
	matData.staticFriction = 0.0f;
	matData.dynamicFriction = 0.0f;
	matData.restitution = 0.0f;

	return data;
}

MaterialData Tremplin::GetMaterialData() const
{
	MaterialData matData;
	matData.staticFriction = 0.0f;
	matData.dynamicFriction = 0.0f;
	matData.restitution = 0.0f;

	return matData;
}

void Tremplin::OnCollisionEnter(GameObject& other)
{
	if (other.GetComponent<PlayerComponent>())
	{
		SoundManager::GetInstance().playSong("resources/sounds/woosh.wav", "woosh", 0, 1);
	}
}
