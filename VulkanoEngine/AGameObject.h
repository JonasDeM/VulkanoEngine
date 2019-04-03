#pragma once
#include "SceneGraphDataManager.h"
#include "ComponentTypeIndexGenerator.h"
#include <type_traits>
class Transform;

struct GameObjectData
{
	// Maybe add Flags? Static | QueuedForDestroy | Enabled | ...

	std::unordered_map<ComponentTypeIndex, SceneGraphDataIndex> m_ComponentMap;
};

// Handle for users to work with
// Contains an index into a container of objects of type GameObjectData
class GameObject1
{
public:
	static GameObject1 CreateNew();

	template<class ComponentType>
	bool HasComponent();
	template<class ComponentType>
	ComponentType AddComponent();
	template<class ComponentType>
	ComponentType GetComponent();

	// More optimized way of doing GetComponent<Transform>()
	Transform GetTransform();
private:
	using GameObjectDataManager = SceneGraphDataManager<GameObjectData>;

	SceneGraphDataIndex m_IndexToAccesData = 0;
};

template<class ComponentType>
bool GameObject1::HasComponent()
{
	//check if it has requested component
	auto& map = GameObjectDataManager::GetData(m_IndexToAccesData).m_ComponentMap;
	return map.find(ComponentType::TypeIndex) != map.end();

}

template<class ComponentType>
ComponentType GameObject1::GetComponent()
{
	//check if it has requested component
	assert(GameObjectDataManager::GetData(m_IndexToAccesData).m_ComponentMap.find(ComponentType::TypeIndex) != GameObjectDataManager::GetData(m_IndexToAccesData).m_ComponentMap.end());
	ComponentType c;
	c.InitializeHandle(*this, GameObjectDataManager::GetData(m_IndexToAccesData).m_ComponentMap.at(ComponentType::TypeIndex));
	return c;
}

template<class ComponentType>
ComponentType GameObject1::AddComponent()
{
	static_assert(sizeof(ComponentType) == sizeof(Component<int>), "Slicing will occur. Components should all be the same size. (most of the time it's just an identifier in 1 or multiple containers)");
	GameObjectData& Data = GameObjectDataManager::GetData(m_IndexToAccesData);
	ComponentType c;
	c.InitializeHandle(*this);
	//check if already has component
	assert(Data.m_ComponentMap.find(ComponentType::TypeIndex) == Data.m_ComponentMap.end());
	Data.m_ComponentMap.insert({ ComponentType::TypeIndex,  c.m_IndexToAccesData });
	return c;
}
