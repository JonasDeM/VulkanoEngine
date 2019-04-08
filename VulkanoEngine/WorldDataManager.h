#pragma once
#include <vector>

class DataManager
{
public:
	DataManager() = default;
	virtual ~DataManager() = default; // enables me to keep pointers to all WorldDataManagers in a container
	DataManager(const DataManager &obj) = delete;
	DataManager& operator=(const DataManager& obj) = delete;
};

// Manages all objects of type HandleType::Datatype in a vector
template<typename HandleType>
class WorldDataManager : public DataManager
{
private:
	using DataType = typename HandleType::DataType;
	using SystemType = typename HandleType::SystemType;
	static_assert(std::is_base_of<ComponentData, DataType>::value, "HandleType::Datatype has to inherit from ComponentData");
public:
	HandleType CreateNew()
	{
		size_t indexOfNew = m_Data.size();
		m_Data.resize(indexOfNew+1);
		HandleType handle = GetByIndex(indexOfNew);
		SystemType::Awake(handle);
		return handle;
	}

	// Only call this if you're sure of having data at that index
	HandleType GetByIndex(size_t index)
	{
		assert(index < m_Data.size());
		HandleType handle;
		handle.InitializeHandle(&m_Data[index]);
		return handle;
	}

	template<typename... Args>
	void UpdateAll(Args... as);

	//template<typename... Args>
	//void UpdateAllAsync(Args... as)

	//template<typename... Args>
	//void RecordVulkanDrawCommands(Args... as);

private:
	std::vector<DataType> m_Data;
	// std::vector<size_t> m_DataToCallStartOnWithItsSystemPleaseRenameMe;
};

template<typename HandleType>
template<typename ...Args>
void WorldDataManager<HandleType>::UpdateAll(Args ...as)
{
	HandleType handle;
	for (DataType& data: m_Data)
	{
		handle.InitializeHandle(&data);
		SystemType::Update(handle, as...);
	}
}