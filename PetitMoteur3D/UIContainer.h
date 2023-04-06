#pragma once
#include "AfficheurSprite.h"
#include "AfficheurTexte.h"

class UIContainer
{
protected:
	PM3D::CAfficheurSprite* afficheur;

public:
	UIContainer() noexcept = default;
	virtual ~UIContainer();

	virtual void Update() = 0;
	virtual void Draw(const CCamera& camera, const CLight& light, const CTransform& transform);

	PM3D::CAfficheurSprite* GetAfficheur() noexcept;
};

class UIPlay : public UIContainer
{
public:
	void Update() override;
};

class UIPause : public UIContainer
{
public:
	void Update() override;
};

class UILoading : public UIContainer
{
public:
	void Update() override;
};

class UISnow : public UIContainer
{
public:
	void Update() override;
};

class UIEndScreen : public UIContainer
{
public:
	void Update() override;
};

class UIResolution : public UIContainer
{
public:
	void Update() override;
};

class UICredits : public UIContainer
{
public:
	void Update() override;
};

