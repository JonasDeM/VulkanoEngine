#pragma once
#include "AGameObject.h"

struct ComponentData
{
public:
	GameObject1 GetGameObject() { return m_GameObject; }
private:
	friend class World;
	GameObject1 m_GameObject;
};