#include "stdafx.h"
#include "World.h"
#include "Transform.h"
#include "ComponentTypeIndexGenerator.h"

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
	GameObject1 g;
	g.InitializeHandle(this, m_pGameObjectDataManager->CreateNew());
	return g;
}

TransformData * World::FastTransformAccess(GameObjectData* pGameObjectData)
{
	int64_t diff = pGameObjectData - m_pGameObjectDataManager->GetData(0);
	return GetManager<Transform>()->GetData(diff);
}

int main(int arc, char** argv)
{
	World* w = new World();
	auto ptr = w->CreateDataFor<Transform>();
	ptr->Update();
	w->Update();
}
