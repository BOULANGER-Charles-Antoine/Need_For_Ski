#include "stdafx.h"
#include "util.h"
#include "Obstacle.h"
#include "ObjetMesh.h"
#include <memory>

using namespace PM3D;

Obstacle::Obstacle(std::string nom,CChargeurOBJ& chargeur) noexcept : StaticGameObject(nom, chargeur) { /*vide*/ }

Obstacle::Obstacle(std::string nom, PM3D::CParametresChargement params) noexcept : StaticGameObject(nom, params) { /*vide*/ }

void Obstacle::InitComponents()
{
	components.push_back(std::make_shared<ObstacleComponent>(*this));
}

void Obstacle::OnTriggerEnter(GameObject& other)
{
}

RigidActorData Obstacle::GetActorData() const
{
	RigidActorData data{ id };
	data.useGravity = false;
	data.isTrigger = false;
	data.layer = Layer::Terrain;

	return data;
}

MaterialData Obstacle::GetMaterialData() const
{
	MaterialData matData;
	matData.staticFriction = 0.0f;
	matData.dynamicFriction = 0.0f;
	matData.restitution = 0.0f;

	return matData;
}
