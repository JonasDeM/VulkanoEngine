#include "stdafx.h"
#include "AGameObject.h"
#include "Component.h"
#include <algorithm>
#include "Transform.h"

GameObject1 GameObject1::CreateNew()
{
	GameObject1 g;
	g.m_IndexToAccesData = GameObjectDataManager::CreateNew();
	return g;
}

Transform GameObject1::GetTransform()
{
	// every gameobject has a transform, so they have the same index in their containers, no lookup in map necessary
	// if that ever changes just delete this method and replace usages with GetComponent<Transform>();
	Transform t;
	t.InitializeHandle(*this, m_IndexToAccesData);
	return t;
}
