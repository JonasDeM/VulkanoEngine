#pragma once
#include "ComponentTypeIndexGenerator.h"
#include <type_traits>
class Transform;
class World;
class GameObjectSystem;
template<typename T>
class WorldDataManager;

struct GameObjectData
{
	// Maybe add Flags? RootObject | Persistent | Static | QueuedForDestroy | Enabled | ...
	std::unordered_map<ComponentTypeIndex, void*> m_ComponentMap;
};

// Handle for users to work with
class GameObject1
{
public:
	using DataType = GameObjectData;
	using SystemType = GameObjectSystem;
public:
	template<class ComponentType>
	bool HasComponent();
	template<class ComponentType>
	ComponentType AddComponent();
	template<class ComponentType>
	ComponentType GetComponent();

	GameObject1 CreateChild();		// Creates a new gameobject as child of this one
	GameObject1 CreateRoot();		// Creates a new root gameobject in this gameobject's world
	GameObject1 CreatePersistent();	// Creates a new root gameobject in this gameobject's world, but will be transferred to a new world when switching to that world.
public:
	bool IsValid() { return m_pData != nullptr; }
	bool operator ==(const GameObject1 &b) const;
	// More optimized way of doing GetComponent<Transform>()
	Transform GetTransform();
private:
	friend class World;
	template<typename T>
	friend class WorldDataManager;
	void InitializeHandle(World* world, GameObjectData* pData);
	GameObjectData* m_pData = nullptr;
	World* m_pWorld = nullptr;
};

template<class ComponentType>
bool GameObject1::HasComponent()
{
	//check if it has requested component
	auto& map = m_pData->m_ComponentMap;
	return map.find(ComponentType::TypeIndex) != map.end();
}

template<class ComponentType>
ComponentType GameObject1::GetComponent()
{
	//check if it has requested component
	assert(m_pData->m_ComponentMap.find(ComponentType::TypeIndex) != m_pData->m_ComponentMap.end());
	ComponentType c;
	c.InitializeHandle(static_cast<ComponentType::DataType*>(m_pData->m_ComponentMap.at(ComponentType::TypeIndex)));
	return c;
}

template<class ComponentType>
ComponentType GameObject1::AddComponent()
{
	static_assert(sizeof(ComponentType) == sizeof(Component<int,int>), "Slicing will occur. Components should all be the same size. (2 pointers)");
	ComponentType c = m_pWorld->CreateComponent<ComponentType>(*this);
	//check if already has component
	assert(m_pData->m_ComponentMap.find(ComponentType::TypeIndex) == m_pData->m_ComponentMap.end());
	m_pData->m_ComponentMap.insert({ ComponentType::TypeIndex,  c.m_pData });
	return c;
}

class GameObjectSystem
{
public:
	static void Build(GameObject1 gameObject) {};
	static void Awake(GameObject1 gameObject) {};
	static void Update(GameObject1 gameObject) {};
	static void RecordVulkanDrawCommands(GameObject1 gameObject) {};
	static void Destroy(GameObject1 gameObject) {};
};
