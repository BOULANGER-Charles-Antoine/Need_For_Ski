#pragma once
#include "StaticGameObject.h"
#include "Component.h"
#include <string>

struct ObstacleComponent : Component
{
	ObstacleComponent(GameObject& gameObject) : Component(gameObject) { /*vide*/ }
};

class Obstacle : public StaticGameObject 
{
public:
	Obstacle(std::string nom, PM3D::CChargeurOBJ& chargeur) noexcept;
	Obstacle(std::string nom, PM3D::CParametresChargement params) noexcept;

protected:
	virtual void InitComponents();

	virtual void OnTriggerEnter(GameObject& other) override;

	RigidActorData GetActorData() const override;
	MaterialData GetMaterialData() const override;
};
