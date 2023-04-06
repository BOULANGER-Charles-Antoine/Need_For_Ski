#pragma once
#include "StaticGameObject.h"
#include "ChargeurOBJ.h"
#include "Component.h"
#include <string>
#include <memory>

const inline std::string TERRAIN_NAME = "Terrain";

class TerrainObject : public StaticGameObject
{
public:
	TerrainObject(PM3D::CChargeurOBJ& chargeur) noexcept;
	TerrainObject(PM3D::CParametresChargement params) noexcept;

protected:
	virtual void InitComponents() override;

	RigidActorData GetActorData() const override;
	MaterialData GetMaterialData() const override;
};

struct TerrainComponent : Component
{
	static RigidActorData GetActorData() { };
	static MaterialData GetMaterialData() { };

	TerrainComponent(TerrainObject& obj) : Component(obj) { /*vide*/ }
};