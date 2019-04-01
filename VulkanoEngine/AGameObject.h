#pragma once
#include "SceneGraphDataManager.h"
#include <type_traits>
class Transform;

// Type used for indexing each Component type
using ComponentTypeIndex = uint16_t;

struct GameObjectData
{
	std::unordered_map<ComponentTypeIndex, uint32_t> m_ComponentMap;
};

class GameObject1
{
	using GameObjectDataManager = SceneGraphDataManager<GameObjectData>;

public:
	static GameObject1 CreateNew();

	template<class ComponentType>
	ComponentType AddComponent();

	template<class ComponentType>
	ComponentType GetComponent();

	template<>
	Transform GetComponent<Transform>();
private:
	friend class Transform;
	uint32_t m_IndexToAccesData = 0;
};

template<class ComponentType>
ComponentType GetComponent()
{
	assert(DataManager::GetData(m_IndexToAccesData)->find() != DataManager::GetData()->end());
	ComponentType c;
	c.InitializeHandle(*this, GameObjectDataManager::GetData(m_IndexToAccesData).m_ComponentMap.at(ComponentType::TypeIndex));
	return c;
}

template<class ComponentType>
ComponentType GameObject1::AddComponent()
{
	//check if already has component
	static_assert(sizeof(ComponentType) == sizeof(Component<int>), "Slicing will occur. Components should all be the same size. (most of the time it's just an identifier in 1 or multiple containers)");
	GameObjectData& Data = GameObjectDataManager::GetData(m_IndexToAccesData);
	ComponentType c;
	c.InitializeHandle(*this);
	Data.m_ComponentMap.insert({ ComponentType::TypeIndex,  c.m_IndexToAccesData });
	return c;
}
