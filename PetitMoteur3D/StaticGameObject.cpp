#include "stdafx.h"
#include "StaticGameObject.h"
#include "PhysXEngine.h"
#include "GestionnaireDeChargeur.h"

using namespace std;
using namespace PM3D;
using namespace physx;
using namespace DirectX;

StaticGameObject::StaticGameObject(std::string nom, const PM3D::CChargeurOBJ& chargeur) noexcept 
	: GameObject(nom), chargeur{chargeur} 
{ 
	//vide 
}

StaticGameObject::StaticGameObject(std::string nom, const PM3D::CParametresChargement params) noexcept 
	: GameObject(nom), chargeur{ CGestionnaireDeChargeurOBJ::GetInstance().GetChargeur(params) }
{
	//vide
}

PM3D::CObjetMesh* StaticGameObject::InitMesh()
{
	return new CObjetMesh(chargeur);
}

physx::PxRigidStatic* StaticGameObject::InitActor()
{
	//Cr�er une liste de sommets
	const size_t nombreSommets = chargeur.GetNombreSommets();
	PxVec3* verts = new PxVec3[nombreSommets];

	for (uint32_t i = 0; i < nombreSommets; ++i)
	{
		XMFLOAT3 pos = chargeur.GetPosition(i);
		verts[i] = PxVec3(pos.x, pos.y, pos.z);
	}

	// Cr�er une liste de triangle
	const vector<int> indexes = chargeur.GetIndex();
	PxU32* inds = new PxU32[indexes.size()];
	for (int i = 0; i < indexes.size(); ++i)
	{
		inds[i] = PxU32(indexes[i]);
	}

	// Cr�er la description du mesh
	PxTriangleMeshDesc desc;
	desc.points.count = nombreSommets;
	desc.points.stride = sizeof(PxVec3);
	desc.points.data = verts;
	desc.triangles.count = indexes.size() / 3;
	desc.triangles.stride = 3 * sizeof(PxU32);
	desc.triangles.data = inds;

	// Prendre le scale de l'objet
	const XMFLOAT3 scale = transform.GetScaleFloat3();
	const PxVec3 pxScale{ scale.x, scale.y, scale.z };

	// Cr�er la geometry
	const PxTriangleMeshGeometry geometry = PhysXEngine::CreateTriangleMesh(desc, pxScale);

	// Cr�er l'actor
	RigidActorData data = GetActorData();
	MaterialData matData = GetMaterialData();
	return PhysXEngine::CreateRigidStatic(data, geometry, matData);
}
