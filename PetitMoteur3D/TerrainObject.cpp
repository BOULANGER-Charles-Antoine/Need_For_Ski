#include "stdafx.h"
#include "TerrainObject.h"
#include "ObjetMesh.h"

#include <string>
#include <memory>

using namespace std;
using namespace PM3D;
using namespace physx;
using namespace DirectX;

TerrainObject::TerrainObject(PM3D::CChargeurOBJ& chargeur) noexcept : StaticGameObject(TERRAIN_NAME, chargeur) 
{ 
	//vide
}

TerrainObject::TerrainObject(PM3D::CParametresChargement params) noexcept : StaticGameObject(TERRAIN_NAME, params) 
{
	//vide
}

void TerrainObject::InitComponents()
{
	components.push_back(make_shared<TerrainComponent>(*this));
}

RigidActorData TerrainObject::GetActorData() const
{
	RigidActorData data{ id };
	data.layer = Layer::Terrain;
	data.useGravity = false;

	return data;
}

MaterialData TerrainObject::GetMaterialData() const
{
	MaterialData matData;
	matData.staticFriction = 0.0f;
	matData.dynamicFriction = 0.0f;
	matData.restitution = 0.0f;

	return matData;
}
