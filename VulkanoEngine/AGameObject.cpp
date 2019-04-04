#include "stdafx.h"
#include "AGameObject.h"
#include "Component.h"
#include <algorithm>
#include "Transform.h"
#include <limits>

GameObject1 GameObject1::CreateNew()
{
	GameObject1 g;
	g.m_IndexToAccesData = GameObjectDataManager::CreateNew();
	assert(g.m_IndexToAccesData < std::numeric_limits<SceneGraphDataIndex>::max());
	g.AddComponent<Transform>();
	return g;
}

// Template specialization was impossible because i can't include "Transform.h" (template specialization needs to be in header)
// (Because this header is included in "Transform.h" via "Component.h")
// If i'm wrong, please contact me with a solution - Jonas De Maeseneer
Transform GameObject1::GetTransform()
{
	// every gameobject has a transform, so they have the same index in their containers, no lookup in map necessary
	// if that ever changes just delete this method and replace usages with GetComponent<Transform>();
	Transform t;
	t.InitializeHandle(*this, m_IndexToAccesData);
	return t;
}
