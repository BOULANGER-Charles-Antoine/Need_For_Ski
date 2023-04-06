#pragma once
#include "GameObject.h"

class GraphicObject : public GameObject
{
public:
	GraphicObject() noexcept : GameObject("GRAPHIC_OBJECT") { /*vide*/ }

protected:
	virtual physx::PxRigidActor* InitActor() { return nullptr; }
};

class HeightmapObject : public GraphicObject
{
	std::wstring heightmap;
	std::wstring foreground;
	std::wstring background;
	std::wstring ground;

public:
	HeightmapObject(std::wstring heightmap, std::wstring foreground, std::wstring background, std::wstring ground) noexcept;

protected:
	PM3D::CObjet3D* InitMesh() override;
};

class SkyboxObject : public GraphicObject
{
	std::wstring skybox;

public:
	SkyboxObject(std::wstring skybox) noexcept;

protected:
	PM3D::CObjet3D* InitMesh() override;
};