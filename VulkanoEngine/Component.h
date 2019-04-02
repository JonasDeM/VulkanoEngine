#pragma once
#include "AGameObject.h"
#include "ComponentTypeIndexGenerator.h"

template<typename DataType>
class Component
{
public:
	GameObject1 GetGameObject() { return m_GameObject; }

	template<class ComponentType>
	ComponentType GetComponent();

protected:
	static const uint16_t TypeIndex;
	using ComponentDataManager = SceneGraphDataManager<DataType>; // I could make SceneGraphDataManager a template parameter aswell, then you could specify a custom datamanager
	uint32_t m_IndexToAccesData = 0;
	GameObject1 m_GameObject;
	friend class GameObject1;

	DataType& GetData();

	// Initialize existing component
	void InitializeHandle(GameObject1 gameObject, uint32_t indexToAccesData);
	// Initialize existing new component
	void InitializeHandle(GameObject1 gameObject);

};

template<typename DataType>
const uint16_t Component<DataType>::TypeIndex = ComponentTypeIndexGenerator::GenerateTypeIndex();

template<typename DataType>
template<class ComponentType>
ComponentType Component<DataType>::GetComponent()
{
	return m_GameObject.GetComponent<ComponentType>();
}

template<typename DataType>
inline DataType & Component<DataType>::GetData()
{
	return ComponentDataManager::GetData(m_IndexToAccesData);
}

template<typename DataType>
void Component<DataType>::InitializeHandle(GameObject1 gameObject, uint32_t indexToAccesData)
{
	m_GameObject = gameObject;
	m_IndexToAccesData = indexToAccesData;
}

template<typename DataType>
void Component<DataType>::InitializeHandle(GameObject1 gameObject)
{
	m_GameObject = gameObject;
	m_IndexToAccesData = ComponentDataManager::CreateNew();
}
