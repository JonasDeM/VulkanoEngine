#pragma once
#include "AGameObject.h"

class World;
template<typename T>
class WorldDataManager;

// Handle for users to work with
// Contains an index into a container of objects of type <Datatype>
template<typename D, typename S>
class Component
{
public:
	using DataType = D;
	using SystemType = S;
public:
	GameObject1 GetGameObject() { return m_pData->GetGameObject(); }

	template<class ComponentType>
	ComponentType GetComponent();

	bool IsValid() { return m_pData != nullptr; }
	bool operator ==(const Component<D, S> &b) const;
protected:
	DataType* m_pData = nullptr;
private:
	friend class World;
	friend class GameObject1;
	template<typename T>
	friend class WorldDataManager;

	static const ComponentTypeIndex TypeIndex;

	void InitializeHandle(DataType* pData);
};

template<typename D, typename S>
const ComponentTypeIndex Component<D,S>::TypeIndex = ComponentTypeIndexGenerator::GenerateTypeIndex();

template<typename D, typename S>
template<class ComponentType>
ComponentType Component<D,S>::GetComponent()
{
	return GetGameObject().GetComponent<ComponentType>();
}

template<typename D, typename S>
bool Component<D,S>::operator==(const Component<D,S>& b) const
{
	return m_pData == b.m_pData;
}

template<typename D, typename S>
void Component<D,S>::InitializeHandle(DataType* pData)
{
	m_pData = pData;
}
