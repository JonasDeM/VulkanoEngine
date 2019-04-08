#include "stdafx.h"
#include "AGameObject.h"
#include "World.h"
#include "Transform.h"
#include <limits>

GameObject1 GameObject1::CreateChild()
{
	return m_pWorld->CreateGameObject(GetTransform());
}

GameObject1 GameObject1::CreateRoot()
{
	return m_pWorld->CreateGameObject();
}

GameObject1 GameObject1::CreatePersistent()
{
	assert(false); // Not Implemented
	return GameObject1();
}

bool GameObject1::operator==(const GameObject1 & b) const
{
	return m_pData == b.m_pData;
}

// Template specialization was impossible because i can't include "Transform.h" 
// (Because this header is included in "Transform.h" via "Component.h") + (template specialization needs to be in header)
// If i'm wrong, please contact me with a solution - Jonas De Maeseneer
Transform GameObject1::GetTransform()
{
	return m_pWorld->FastTransformAccess(m_pData);
}

void GameObject1::InitializeHandle(World * world, GameObjectData * pData)
{
	m_pWorld = world;
	m_pData = pData;
}
