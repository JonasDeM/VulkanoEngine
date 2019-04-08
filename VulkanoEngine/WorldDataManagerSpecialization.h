#pragma once
#include "WorldDataManager.h"
#include "AGameObject.h"

class World;

template<>
class WorldDataManager<GameObject1>
{
private:
	using HandleType = GameObject1;
	using DataType = GameObject1::DataType;
	using SystemType = GameObject1::SystemType;
public:
	HandleType CreateNew(World* pWorld)
	{
		m_Data.resize(m_Data.size() + 1);
		HandleType handle;
		handle.InitializeHandle(pWorld, &m_Data[m_Data.size() - 1]);
		SystemType::Awake(handle);
		return handle;
	}

	// Only call this if you're sure of having 1 element in this manager
	DataType* GetAddressOffFirst()
	{
		assert(m_Data.size() > 0);
		return &m_Data[0];
	}

private:
	std::vector<DataType> m_Data;
};