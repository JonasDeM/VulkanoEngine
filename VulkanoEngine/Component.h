#pragma once
#include "AGameObject.h"

// Handle for users to work with
// Contains an index into a container of objects of type <Datatype>
template<typename DataType>
class Component
{
public:
	GameObject1 GetGameObject() { return m_GameObject; }

	template<class ComponentType>
	ComponentType GetComponent();

	bool IsValid() { return m_IndexToAccesData == 0; };
	bool operator ==(const Component<DataType> &b) const;
protected:
	// Method to get a reference to the actual DataType object
	DataType& GetData();

private:
	friend class GameObject1;
	static const ComponentTypeIndex TypeIndex;
	using ComponentDataManager = SceneGraphDataManager<DataType>; // I could make SceneGraphDataManager a template parameter aswell, then you could specify a custom datamanager

	// Initialize existing component
	void InitializeHandle(GameObject1 gameObject, SceneGraphDataIndex indexToAccesData);
	// Initialize existing new component
	void InitializeHandle(GameObject1 gameObject);

	SceneGraphDataIndex m_IndexToAccesData = 0;
	GameObject1 m_GameObject;
};

template<typename DataType>
const ComponentTypeIndex Component<DataType>::TypeIndex = ComponentTypeIndexGenerator::GenerateTypeIndex();

template<typename DataType>
template<class ComponentType>
ComponentType Component<DataType>::GetComponent()
{
	return m_GameObject.GetComponent<ComponentType>();
}

template<typename DataType>
bool Component<DataType>::operator==(const Component<DataType>& b) const
{
	return m_IndexToAccesData == b.m_IndexToAccesData;
}

template<typename DataType>
inline DataType & Component<DataType>::GetData()
{
	return ComponentDataManager::GetData(m_IndexToAccesData);
}

template<typename DataType>
void Component<DataType>::InitializeHandle(GameObject1 gameObject, SceneGraphDataIndex indexToAccesData)
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
