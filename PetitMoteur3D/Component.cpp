#include "stdafx.h"
#include "Component.h"

CTransform& Component::GetTransform()
{
    return gameObject.GetTransform();
}

Component::Component(GameObject& gameObject) : gameObject{gameObject}
{
}
