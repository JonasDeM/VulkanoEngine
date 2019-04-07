#include "stdafx.h"
#include "AGameObject.h"
#include "World.h"
#include "Transform.h"
#include <limits>

//GameObject1 GameObject1::CreateNew()
//{
//	GameObject1 g;
//	g.m_IndexToAccesData = GameObjectDataManager::CreateNew();
//	assert(g.m_IndexToAccesData < std::numeric_limits<SceneGraphDataIndex>::max());
//	g.AddComponent<Transform>();
//	return g;
//}

bool GameObject1::operator==(const GameObject1 & b) const
{
	return m_pData == b.m_pData;
}

// Template specialization was impossible because i can't include "Transform.h" 
// (Because this header is included in "Transform.h" via "Component.h") + (template specialization needs to be in header)
// If i'm wrong, please contact me with a solution - Jonas De Maeseneer
Transform GameObject1::GetTransform()
{
	Transform t;
	t.InitializeHandle(*this, m_pWorld->FastTransformAccess(m_pData));
	return t;
}

void GameObject1::InitializeHandle(World * world, GameObjectData * pData)
{
	m_pWorld = world;
	m_pData = pData;
}
