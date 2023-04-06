#pragma once
#include "GameObject.h"
#include "ChargeurOBJ.h"
#include "ObjetMesh.h"

struct RigidActorData;
struct MaterialData;

class StaticGameObject : public GameObject
{
	const PM3D::CChargeurOBJ& chargeur;

public:
	StaticGameObject(std::string nom, const PM3D::CChargeurOBJ& chargeur) noexcept;
	StaticGameObject(std::string nom, const PM3D::CParametresChargement params) noexcept;

protected:
	virtual PM3D::CObjetMesh* InitMesh() override;
	virtual physx::PxRigidStatic* InitActor() override;

	virtual RigidActorData GetActorData() const = 0;
	virtual MaterialData GetMaterialData() const = 0;
};

