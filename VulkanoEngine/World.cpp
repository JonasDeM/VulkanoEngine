#include "stdafx.h"
#include "World.h"
#include "ComponentTypeIndexGenerator.h"

#include "Transform.h"
#include "TransformData.h"
#include "TransformSystem.h"

World::World() :
	m_ComponentDataManagers(ComponentTypeIndexGenerator().GetAmountComponentTypes())
{
	m_pGameObjectDataManager = std::make_unique<WorldDataManager<GameObject1>>();
	RegisterComponentType<Transform>();
}

void World::Update()
{
	GetManager<Transform>()->UpdateAll();
}

GameObject1 World::CreateGameObject()
{
	GameObject1 g = m_pGameObjectDataManager->CreateNew(this);
	Transform t = g.AddComponent<Transform>();
	return g;
}

GameObject1 World::CreateGameObject(Transform parent)
{
	GameObject1 g = m_pGameObjectDataManager->CreateNew(this);
	Transform t = g.AddComponent<Transform>();
	t.SetParent(parent);
	return g;
}

Transform World::FastTransformAccess(GameObjectData* pGameObjectData)
{
	return GetManager<Transform>()->GetByIndex(pGameObjectData - m_pGameObjectDataManager->GetAddressOffFirst());
}

int main(int arc, char** argv)
{
	World* w = new World();
	auto handle = w->CreateComponent<Transform>(w->CreateGameObject());
}
