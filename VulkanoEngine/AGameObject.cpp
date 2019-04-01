#include "stdafx.h"
#include "AGameObject.h"
#include "Component.h"
#include <algorithm>
#include "Transform.h"

template<>
Transform GameObject1::GetComponent<Transform>()
{
	Transform t;
	t.InitializeHandle(*this, m_IndexToAccesData);
}

GameObject1 GameObject1::CreateNew()
{
	GameObject1 g;
	g.m_IndexToAccesData = GameObjectDataManager::CreateNew();
	return g;
}
