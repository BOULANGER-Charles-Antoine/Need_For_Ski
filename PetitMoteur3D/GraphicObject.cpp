#include "stdafx.h"
#include "GraphicObject.h"
#include "CTerrain.h"
#include "Skybox.h"

using namespace PM3D;

#pragma region Heightmap

HeightmapObject::HeightmapObject(std::wstring heightmap, std::wstring foreground, std::wstring background, std::wstring ground) noexcept
	: GraphicObject(), heightmap{heightmap}, foreground{foreground}, background{background}, ground{ground}
{
	//Vide
}

PM3D::CObjet3D* HeightmapObject::InitMesh()
{
	CTerrain* terrain = new CTerrain(heightmap);
	terrain->SetBackground(background);
	terrain->SetForeground(foreground);
	terrain->SetGround(ground);

	return terrain;
}

#pragma endregion

#pragma region  Skybox

SkyboxObject::SkyboxObject(std::wstring skybox) noexcept : GraphicObject(), skybox{skybox}
{
	//Vide
}

PM3D::CObjet3D* SkyboxObject::InitMesh()
{
	Skybox* skyboxObj = new Skybox(skybox);
	skyboxObj->Load();

	return skyboxObj;
}

#pragma endregion