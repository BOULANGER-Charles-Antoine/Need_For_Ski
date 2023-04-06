#pragma once
#include "Singleton.h"

template <class T> class Manager : public CSingleton<T>
{
public:
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Close() = 0;
};

