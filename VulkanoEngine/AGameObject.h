#pragma once
#include "WorldDataManager.h"
#include "ComponentTypeIndexGenerator.h"
#include <type_traits>
class Transform;
class World;

struct GameObjectData
{
	// Maybe add Flags? Static | QueuedForDestroy | Enabled | ...
	void Build() {};
	void Start() {};
	void Update() {};
	void RecordVulkanDrawCommands() {};
	void Stop() {};
	void Destroy() {};

	std::unordered_map<ComponentTypeIndex, void*> m_ComponentMap;
};

// Handle for users to work with
// Contains an index into a container of objects of type GameObjectData
class GameObject1
{
public:
	using DataType = GameObjectData;
public:
	template<class ComponentType>
	bool HasComponent();
	template<class ComponentType>
	ComponentType AddComponent();
	template<class ComponentType>
	ComponentType GetComponent();
public:
	bool IsValid() { return m_pData != nullptr; }
	bool operator ==(const GameObject1 &b) const;
	// More optimized way of doing GetComponent<Transform>()
	Transform GetTransform();
private:
	friend class World;
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
	c.InitializeHandle(*this, static_cast<ComponentType::DataType*>(m_pData->m_ComponentMap.at(ComponentType::TypeIndex)));
	return c;
}

template<class ComponentType>
ComponentType GameObject1::AddComponent()
{
	static_assert(sizeof(ComponentType) == sizeof(Component<int>), "Slicing will occur. Components should all be the same size. (2 pointers)");
	ComponentType c;
	c.InitializeHandle(*this, m_pWorld->CreateData<ComponentType::DataType>());
	//check if already has component
	assert(m_pData.m_ComponentMap.find(ComponentType::TypeIndex) == m_pData->m_ComponentMap.end());
	m_pData->m_ComponentMap.insert({ ComponentType::TypeIndex,  c.m_pData });
	return c;
}
