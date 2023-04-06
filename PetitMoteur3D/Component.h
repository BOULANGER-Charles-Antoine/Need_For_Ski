#pragma once
#include "GameObject.h"

class Component
{
	friend class GameObject;

protected:
	GameObject& gameObject;

	CTransform& GetTransform();

public:
	Component(GameObject& gameObject);
	virtual ~Component() { };

private:
	virtual void Start() { /*À IMPLÉMENTER*/ }
	virtual void Update() { /*À IMPLÉMENTER*/ }
	virtual void OnCollisionEnter(GameObject& other) { /*À IMPLÉMENTER*/ }
	virtual void OnCollisionStay(GameObject& other) { /*À IMPLÉMENTER*/ }
	virtual void OnCollisionExit(GameObject& other) { /*À IMPLÉMENTER*/ }
	virtual void OnTriggerEnter(GameObject& other) { /*À IMPLÉMENTER*/ }
	virtual void OnTriggerStay(GameObject& other) { /*À IMPLÉMENTER*/ }
	virtual void OnTriggerExit(GameObject& other) { /*À IMPLÉMENTER*/ }
};

