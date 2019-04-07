#pragma once

#include "WorldDataManager.h"
class GameObject1;
struct GameObjectData;
class Transform;
struct TransformData;

// Basically a datamanager-manager
class World
{
public:
	World();
	virtual ~World() = default;
	World(const World &obj) = delete;
	World& operator=(const World& obj) = delete;

	virtual void Update();

	GameObject1 CreateGameObject();

	template<class ComponentType>
	typename ComponentType::DataType* CreateDataFor();

	TransformData* FastTransformAccess(GameObjectData* pGameObjectData);
private:
	template<class ComponentType>
	void RegisterComponentType();

	template<class ComponentType>
	WorldDataManager<ComponentType>* GetManager();

	template<>
	WorldDataManager<GameObject1>* GetManager();

	std::unique_ptr<WorldDataManager<GameObject1>> m_pGameObjectDataManager = nullptr;
	std::vector<std::unique_ptr<DataManager>> m_ComponentDataManagers;
};

template<class ComponentType>
void World::RegisterComponentType()
{
	assert(ComponentType::TypeIndex != std::numeric_limits<ComponentTypeIndex>::max());
	m_ComponentDataManagers[ComponentType::TypeIndex] = std::make_unique<WorldDataManager<ComponentType>>();
}

template<class ComponentType>
WorldDataManager<ComponentType>* World::GetManager()
{
	assert(ComponentType::TypeIndex < m_ComponentDataManagers.size()); // should never happen because vector is initialized so its size equals the amount of ComponentTypes
	assert(m_ComponentDataManagers[ComponentType::TypeIndex] == nullptr); // ComponentType is not registered
	return static_cast<WorldDataManager<ComponentType>*>(m_ComponentDataManagers[ComponentType::TypeIndex].get());
}

template<>
WorldDataManager<GameObject1>* World::GetManager()
{
	return m_pGameObjectDataManager.get();
}

template<class ComponentType>
typename ComponentType::DataType * World::CreateDataFor()
{
	assert(ComponentType::TypeIndex < m_ComponentDataManagers.size()); // should never happen because vector is initialized so its size equals the amount of ComponentTypes
	assert(m_ComponentDataManagers[ComponentType::TypeIndex] == nullptr); // ComponentType is not registered
	return static_cast<WorldDataManager<ComponentType>*>(m_ComponentDataManagers[ComponentType::TypeIndex].get())->CreateNew();
}


