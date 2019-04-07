#pragma once
#include "AGameObject.h"

class World;

// Handle for users to work with
// Contains an index into a container of objects of type <Datatype>
template<typename T>
class Component
{
public:
	using DataType = T;
public:
	GameObject1 GetGameObject() { return m_GameObject; }

	template<class ComponentType>
	ComponentType GetComponent();

	bool IsValid() { return m_pData != nullptr; }
	bool operator ==(const Component<DataType> &b) const;
protected:
	friend class World;
	DataType* m_pData = nullptr;
private:
	friend class GameObject1;
	static const ComponentTypeIndex TypeIndex;

	void InitializeHandle(GameObject1 gameObject, DataType* pData);

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
	return m_pData == b.m_pData;
}

template<typename DataType>
void Component<DataType>::InitializeHandle(GameObject1 gameObject, DataType* pData)
{
	m_GameObject = gameObject;
	m_pData = pData;
}
