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
	virtual void Start() { /*� IMPL�MENTER*/ }
	virtual void Update() { /*� IMPL�MENTER*/ }
	virtual void OnCollisionEnter(GameObject& other) { /*� IMPL�MENTER*/ }
	virtual void OnCollisionStay(GameObject& other) { /*� IMPL�MENTER*/ }
	virtual void OnCollisionExit(GameObject& other) { /*� IMPL�MENTER*/ }
	virtual void OnTriggerEnter(GameObject& other) { /*� IMPL�MENTER*/ }
	virtual void OnTriggerStay(GameObject& other) { /*� IMPL�MENTER*/ }
	virtual void OnTriggerExit(GameObject& other) { /*� IMPL�MENTER*/ }
};

